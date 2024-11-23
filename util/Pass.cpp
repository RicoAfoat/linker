#include "Pass.h"
#include "ObjectFile.h"
#include "InputSection.h"
#include "Symbol.h"
#include "Context.h"
#include "Singleton.h"
#include "mergeablesection.h"
#include "SectionFragment.h"
#include <queue>
#include <cassert>
#include <cstring>

void Passes::resolveSymbols(){
    auto& Ctx=Singleton<Context>();

    for(auto& obj:Ctx.Objs)
        obj->resolveSymbols();
    for(auto& obj:Ctx.ExtractObjs)
        obj->resolveSymbols();

    Passes::markLiveObjects();

    for(auto& obj:Ctx.ExtractObjs)
        if(obj->isAlive)
            Ctx.Objs.emplace_back(obj.release());

    Ctx.ExtractObjs.clear();

    std::cerr<<"All live files count:"<<Ctx.Objs.size()<<std::endl;
}

void Passes::markLiveObjects(){
    std::queue<ObjectFile*> que;
    for(auto& obj:Singleton<Context>().Objs)
        que.push(obj.get());
    
    assert(!que.empty());
    
    while(!que.empty()){
        auto obj=que.front();que.pop();
        std::cerr<<"Marking live objects:"<<obj->getFileName()<<std::endl;
        obj->markLiveObjects([&que](ObjectFile* _obj){
            que.push(_obj);
        });
    }
}

void Passes::registerSectionPieces(){
    auto& Ctx=Singleton<Context>();
    for(auto& obj:Ctx.Objs)
        obj->registerSectionPieces();
}

void Passes::createSyntheticSections(){
    auto& Ctx=Singleton<Context>();
    Ctx.Chunks.push_back(&Ctx.OutEhdr);
    Ctx.Chunks.push_back(&Ctx.OutPhdr);
    Ctx.Chunks.push_back(&Ctx.OutShdr);
    Ctx.Chunks.push_back(&Ctx.GotSec);
}

uint64_t Passes::setOutputSectionOffsets(){
    auto& Ctx=Singleton<Context>();
    uint64_t Addr=IMAGE_BASE;
    for(auto& chunk:Ctx.Chunks){
        auto shdr=chunk->getShdr();

        if((shdr->sh_flags&SHF_ALLOC)==0)
            continue;

        Addr=AlignTo(Addr,shdr->sh_addralign);
        shdr->sh_addr=Addr;

        if(!isTbss(chunk))
            Addr+=shdr->sh_size;
    }

    {
        auto first=Ctx.Chunks[0]->getShdr();
        decltype(Ctx.Chunks.size()) i=0,limi=Ctx.Chunks.size();
        for(;;){
            auto shdr=Ctx.Chunks[i]->getShdr();
            shdr->sh_offset=shdr->sh_addr-first->sh_addr;
            i++;
            if(i>=limi||(Ctx.Chunks[i]->getShdr()->sh_flags&SHF_ALLOC)==0)
                break;
        }
        auto last=Ctx.Chunks[i-1]->getShdr();
        auto size=last->sh_offset+last->sh_size;

        for(;i<limi;i++){
            auto shdr=Ctx.Chunks[i]->getShdr();
            size=AlignTo(size,shdr->sh_addralign);
            shdr->sh_offset=size;
            size+=shdr->sh_size;
        }

        for(auto chunk:Ctx.Chunks){
            std::cerr<<chunk->getName()<<" "<<chunk->getShdr()->sh_offset<<" "<<chunk->getShdr()->sh_size<<std::endl;
        }

        Ctx.OutPhdr.UpdateShdr();
        return size;
    }
}

void WriteMagic(void* dst){
    char Magic[4]={0x7f,'E','L','F'};
    memcpy(dst,Magic,4);
}

void Passes::writeOutputFile(){
    auto& Ctx=Singleton<Context>();

    auto filesize=Passes::setOutputSectionOffsets();
    std::cerr<<
    "final size:"<<filesize<<std::endl;
    Ctx.OutputBuf.resize(filesize);

    std::cerr<<"chunks count:"<<Ctx.Chunks.size()<<std::endl;

    for(int i=3;i<4;i++){
        std::cerr<<Ctx.Chunks[i]->getName()<<std::endl;
        Ctx.Chunks[i]->CopyBuf();
    }
    
    // for(auto& chunk:Ctx.Chunks)
        // chunk->CopyBuf();

    std::ofstream file(Ctx.OutputFile,std::ios::binary);
    assert(file.is_open());
    file.write((char*)Ctx.OutputBuf.data(),Ctx.OutputBuf.size());
}

void Passes::collectOutputSections() {
    auto& Chunks=Singleton<Context>().Chunks;
    auto& Osecs=Singleton<Context>().OutSections;
    for(auto& osec:Osecs)
        if(osec->Member.size()!=0)
            Chunks.push_back(osec.get());

    auto& Msecs=Singleton<Context>().mergedSections;
    for(auto& msec:Msecs)
        if(msec->getShdr()->sh_size!=0)
            Chunks.push_back(msec.get());
    return;
}

// We need bin sections bacause we will need to delete some of irrelevant sections after resolve symbols
void Passes::BinSections(){
    auto& Ctx=Singleton<Context>();
    for(auto& Obj:Ctx.Objs){
        auto& Sections=Obj->Sections;
        for(auto& isec:Sections){
            if(isec==nullptr||!isec->isAlive)
                continue;

            isec->OutSec=OutputSection::getOutputSection(std::string(isec->getName()),isec->getShdr()->sh_type,isec->getShdr()->sh_flags);

            isec->OutSec->Member.push_back(isec.get());
        }
    }
}

void Passes::computeSectionSizes(){
    auto& Osecs=Singleton<Context>().OutSections;
    for(auto& osec:Osecs){
        uint64_t offset=0;uint8_t p2align=0;
        for(auto isec:osec->Member){
            offset=AlignTo(offset,uint64_t(1)<<isec->P2Align);
            isec->offset=offset;
            offset+=isec->Content.second;
            p2align=std::max(p2align,isec->P2Align);
        }
        osec->getShdr()->sh_size=offset;
        osec->getShdr()->sh_addralign=uint64_t(1)<<p2align;
    }
}

void Passes::sortOutputSections(){
    auto& chunks=Singleton<Context>().Chunks;
    std::sort(chunks.begin(),chunks.end(),[](Chunk* a,Chunk* b){
        auto ranka=a->rank();
        auto rankb=b->rank();
        if(ranka==rankb){
            return a->getName()<b->getName();
        }
        return ranka<rankb;
    });
}

void Passes::computeMergedSectionSizes(){
    auto& Msecs=Singleton<Context>().mergedSections;
    for(auto& msec:Msecs)
        msec->AssignOffsets();
}

void Passes::scanRelocations(){
    for(auto& obj:Singleton<Context>().Objs)
        obj->scanRelocations();

    auto syms_worklist=std::vector<Symbol*>();
    for(auto& obj:Singleton<Context>().Objs)
        for(auto sym:obj->Symbols){
            if(sym->File==obj.get()&&sym->Flags!=0){
                syms_worklist.push_back(sym);
            }
        }

    for(auto sym:syms_worklist){
        if(sym->Flags&CustomSymbolFlags::NeedsGotTp)
            Singleton<Context>().GotSec.addGotTpSym(sym);
        sym->Flags=0;
    }
}

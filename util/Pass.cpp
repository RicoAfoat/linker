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
    Ctx.Chunks.push_back(&Ctx.OutShdr);
}

uint64_t Passes::setOutputSectionOffsets(){
    auto& Ctx=Singleton<Context>();
    uint64_t size=0;
    // std::sort(Ctx.Chunks.begin(),Ctx.Chunks.end(),[](Chunk* a,Chunk* b){
        // return a->getName()<b->getName();
    // });
    for(auto& chunk:Ctx.Chunks){
        auto shdr=chunk->getShdr();
        size=AlignTo(size,shdr->sh_addralign);
        // std::cerr<<chunk->getName()<<" "<<size<<std::endl;
        shdr->sh_offset=size;
        size+=shdr->sh_size;
    }
    return size;
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

    for(auto& chunk:Ctx.Chunks)
        chunk->CopyBuf();

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

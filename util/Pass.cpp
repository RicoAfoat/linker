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
    Ctx.Chunks.push_back(&Ctx.Ehdr);
}

uint64_t Passes::getOutputFileSize(){
    auto& Ctx=Singleton<Context>();
    uint64_t size=0;
    for(auto& chunk:Ctx.Chunks){
        auto shdr=chunk->getShdr();
        size=AlignTo(size,shdr->sh_addralign)+shdr->sh_size;
    }
    return size;
}

void WriteMagic(void* dst){
    char Magic[4]={0x7f,'E','L','F'};
    memcpy(dst,Magic,4);
}

void Passes::writeOutputFile(){
    auto& Ctx=Singleton<Context>();

    auto filesize=Passes::getOutputFileSize();
    Ctx.OutputBuf.resize(filesize);

    for(auto& chunk:Ctx.Chunks)
        chunk->CopyBuf();

    std::ofstream file(Ctx.OutputFile,std::ios::binary);
    assert(file.is_open());
    file.write((char*)Ctx.OutputBuf.data(),Ctx.OutputBuf.size());
}

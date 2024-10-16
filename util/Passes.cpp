#include "Passes.h"
#include <queue>
using namespace PASSES;

void PASSES::resolveSymbols(Context& Ctx){
    for(auto &EObj:Ctx.ExtractObjs)
        EObj->resolveSymbolsInExtractFiles(Ctx);
    
    std::queue<ObjectFile*> root;
    for(auto& Obj:Ctx.Objs)
        root.push(Obj.get());
    
}

void PASSES::run(Context& Ctx){
    for(auto& Obj:Ctx.ObjectFiles){
        std::cerr<<"Read direct object file:"<<Obj<<std::endl;
        auto Objfile=ObjectFile::OpenWith(Obj);
        assert(Objfile!=nullptr&&"Object file not found");
        Ctx.Objs.emplace_back(Objfile);
    }
    
    resolveSymbols(Ctx);
    // initialize liveness
    // for(auto& Obj:Ctx.Objs)
    //     if(Obj->getArchiveFile()==nullptr)
    //         std::cerr<<"!!!Find live file!!!:"<<Obj->getFileName()<<std::endl;
}
#include "Passes.h"
#include <queue>
using namespace PASSES;

void PASSES::resolveSymbols(Context& Ctx){
    for(auto &EObj:Ctx.ExtractObjs)
        EObj->resolveSymbolsInExtractFiles(Ctx);
    
    auto getSymbolDefinition=[&Ctx](std::string Name){
        if(Ctx.FinalSymbolTable.find(Name)!=Ctx.FinalSymbolTable.end())
            return Ctx.FinalSymbolTable[Name];
        if(Ctx.ArchiveSymbolTable.find(Name)!=Ctx.ArchiveSymbolTable.end())
            return Ctx.ArchiveSymbolTable[Name];
        std::cerr<<"Undefined Reference:"<<Name<<std::endl;
        exit(-1);
    };
    
    for(auto& Obj:Ctx.Objs)
        Obj->resolveSymbols(Ctx);
    
    for(auto& UndefRef:Ctx.UndefSymbols){
        auto SymDef=getSymbolDefinition(UndefRef);
        std::cerr<<"Find reference "<<UndefRef<<std::endl;
        if(SymDef.Obj->getLiveness()==false)
            SymDef.Obj->resolveSymbols(Ctx);
    }
}

void PASSES::run(Context& Ctx){
    for(auto& Obj:Ctx.ObjectFiles){
        std::cerr<<"Read direct object file:"<<Obj<<std::endl;
        auto Objfile=ObjectFile::OpenWith(Obj);
        assert(Objfile!=nullptr&&"Object file not found");
        Objfile->getLiveness()=true;
        Ctx.Objs.emplace_back(Objfile);
    }
    
    resolveSymbols(Ctx);
    // initialize liveness
    // for(auto& Obj:Ctx.Objs)
    //     if(Obj->getArchiveFile()==nullptr)
    //         std::cerr<<"!!!Find live file!!!:"<<Obj->getFileName()<<std::endl;
}
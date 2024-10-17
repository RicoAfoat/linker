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
        std::cerr<<"\33[31mUndefined Reference:"<<Name<<"\33[0m"<<std::endl;
        // exit(-1);
        /* 有些符号定义在 linker script 里，炸掉是技术性调整 */
        return SymbolEntry{nullptr,nullptr,nullptr};
    };
    
    for(auto& Obj:Ctx.Objs)
        Obj->resolveSymbols(Ctx);
    
    while(!Ctx.UndefSymbols.empty()){
        auto UndefRef=Ctx.UndefSymbols.front();
        Ctx.UndefSymbols.pop();
        auto SymDef=getSymbolDefinition(UndefRef);
        if(SymDef.ESym==nullptr&&SymDef.Obj==nullptr&&SymDef.Shdr==nullptr)continue;
        std::cerr<<"Find reference "<<UndefRef<<std::endl;
        if(SymDef.Obj->getLiveness()==false){
            SymDef.Obj->getLiveness()=true;
            SymDef.Obj->resolveSymbols(Ctx);
        }
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
// #include "Passes.h"
// #include <queue>
// using namespace PASSES;

// void PASSES::resolveSymbols(Context& Ctx){
//     for(auto &EObj:Ctx.ExtractObjs)
//         EObj->resolveSymbolsInExtractFiles(Ctx);
    
//     auto getSymbolDefinition=[&Ctx](std::string Name){
//         if(Ctx.FinalSymbolTable.find(Name)!=Ctx.FinalSymbolTable.end())
//             return Ctx.FinalSymbolTable[Name];
//         if(Ctx.ArchiveSymbolTable.find(Name)!=Ctx.ArchiveSymbolTable.end())
//             return Ctx.ArchiveSymbolTable[Name];
//         std::cerr<<"\33[31mUndefined Reference:"<<Name<<"\33[0m"<<std::endl;
//         // exit(-1);
//         /* 有些符号定义在 linker script 里，炸掉是技术性调整 */
//         return SymbolEntry{nullptr,nullptr,nullptr};
//     };
    
//     for(auto& Obj:Ctx.Objs)
//         Obj->resolveSymbols(Ctx);
    
//     while(!Ctx.UndefSymbols.empty()){
//         auto UndefRef=Ctx.UndefSymbols.front();
//         Ctx.UndefSymbols.pop();
//         auto SymDef=getSymbolDefinition(UndefRef);
//         if(SymDef.ESym==nullptr&&SymDef.Obj==nullptr&&SymDef.Shdr==nullptr)continue;
//         std::cerr<<"Find reference "<<UndefRef<<std::endl;
//         if(SymDef.Obj->getLiveness()==false){
//             SymDef.Obj->getLiveness()=true;
//             SymDef.Obj->resolveSymbols(Ctx);
//         }
//     }

//     uint32_t CntLive=0;
    
//     for(auto& Obj:Ctx.Objs){
//         assert(Obj->getLiveness()&&"Object file not live");
//         std::cerr<<"!!!Find live file!!!:"<<Obj->getFileName()<<std::endl;
//         CntLive++;
//     }

//     for(auto& Obj:Ctx.ExtractObjs){
//         if(Obj->getLiveness()){
//             std::cerr<<"!!!Find live file!!!:"<<Obj->getFileName()<<std::endl;
//             CntLive++;
//         }
//     }
//     std::cerr<<"Total live files:"<<CntLive<<std::endl;
// }

// void PASSES::registerSectionPieces(Context& Ctx){
//     for(auto& Obj:Ctx.Objs)
//         Obj->registerSectionPieces(Ctx);
//     for(auto& Obj:Ctx.ExtractObjs)
//         Obj->registerSectionPieces(Ctx);
// }

// void PASSES::run(Context& Ctx){
//     for(auto& Obj:Ctx.ObjectFiles){
//         std::cerr<<"Read direct object file:"<<Obj<<std::endl;
//         auto Objfile=FileBuffer::OpenWith<ObjectFile>(Obj);
//         assert(Objfile!=nullptr&&"Object file not found");
//         Objfile->isAlive=true;
//         Ctx.Objs.emplace_back(Objfile);
//     }
    
//     resolveSymbols(Ctx);
//     registerSectionPieces(Ctx);
// }
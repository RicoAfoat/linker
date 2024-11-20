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

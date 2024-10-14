#include "ArgsParser.h"
#include "Singleton.h"
#include <vector>
#include <memory>
#include <iostream>
#include <cstring>
#include <cassert>

using namespace ArgParser;

std::vector<std::string> ArgParser::getOptionDashform(std::string Name){
    if(Name.size()==1)return {"-"+Name};
    return {"-"+Name,"--"+Name};
}

ExpectArgStatus ExpectedInputFileArgs::operator()(char* arg){
    Singleton<Context>().ObjectFiles.push_back(arg);
    return MatchSuccess;
}

ExpectArgStatus ExpectedFlag::operator()(char* arg){
    for(auto Option: getOptionDashform(getName()))
        if(Option==arg)
            return onFollow(arg);
    return MatchFail;
}

ExpectArgStatus ExpectedWithFollow::operator()(char* arg){
    if(stage==MatchContinue){
        stage=MatchFail;
        return onFollow(arg);
    }
    
    for(auto& Option: getOptionDashform(getName())){
        if(Option==arg){
            stage=MatchContinue;
            break;
        }
    }
    return stage;
}

ExpectArgStatus ExpectedWithPrefix::operator()(char* arg){
    for(auto Option: getOptionDashform(getName()))
        if(strncmp(Option.c_str(),arg,Option.size())==0)
            return onFollow(arg+Option.size());
    return MatchFail;
}


void ArgParser::parse(int argc,char** argv){
    std::cerr<<"DumpArgs:"<<std::endl;
    std::cerr<<"[";
    for(int i=1;i<argc;i++)
        std::cerr<<"\""<<argv[i]<<"\",";
    std::cerr<<"]"<<std::endl<<std::endl;
    
    auto& Ctx=Singleton<Context>();

    std::vector<ExpectArg*> parseItem={
        #include "../conf/ArgParser.def"
    };

    ExpectArg* CurrentPattern=nullptr;
    for(int i=1;i<argc;i++){
        if(CurrentPattern!=nullptr){
            switch((*CurrentPattern)(argv[i])){
            default:
            case MatchContinue:
                [[fallthrough]];
            case MatchFail:
                assert(0&&"unexpected");
            case MatchSuccess:
                CurrentPattern=nullptr;
                break;
            }
            continue;
        }
        for(auto item:parseItem){
            auto Result=(*item)(argv[i]);
            switch (Result)
            {
            case MatchContinue:
                CurrentPattern=item;
            default:
                break;
            }
            if(Result!=MatchFail)break;
        }
    }
    assert(CurrentPattern==nullptr&&"Expecting more arguments");

    dumpContext();

    for(auto item:parseItem)
        delete item;
}

void ArgParser::dumpContext(){
    auto& Ctx=Singleton<Context>();
    
    std::cerr<<"Output File: \n"<<Ctx.OutputFile<<std::endl<<std::endl;
    
    std::cerr<<"Remaining Args: "<<std::endl;
    for(auto& File:Ctx.ObjectFiles)
        std::cerr<<File<<" ";
    std::cerr<<std::endl<<std::endl;
    
    std::cerr<<"Library Paths: "<<std::endl;
    for(auto &LibPath:Ctx.LibraryPaths)
        std::cerr<<LibPath<<" ";
    std::cerr<<std::endl<<std::endl;

    std::cerr<<"ArchiveFiles: "<<std::endl;
    for(auto &LibPath:Ctx.ArchiveFiles)
        std::cerr<<LibPath<<" ";
    std::cerr<<std::endl<<std::endl;
}

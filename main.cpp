#include "./util/ArgsParser.h"
#include "./util/ExtractArchive.h"
#include "./util/Singleton.h"
#include "./util/Context.h"
#include "./util/Pass.h"
#include "./util/Symbol.h"
#include "./util/InputSection.h"
#include <iostream>
int main(int argc,char **argv){
    std::cerr<<"--------------------"<<std::endl;
    std::cerr<<"---custom ld used---"<<std::endl;
    std::cerr<<"--------------------"<<std::endl;
    std::cerr<<std::endl;
    
    ArgParser::parse(argc,argv);
    UnzipArchiveFiles::unzip();
    
    auto& Ctx=Singleton<Context>();
    for(auto objfilename:Ctx.ObjectFiles){
        std::cerr<<"Reading Object File:"<<objfilename<<"\n";
        auto obj=FileBuffer::OpenWith<ObjectFile>(objfilename);
        obj->isAlive=true;
        Ctx.Objs.emplace_back(obj);
    }

    Passes::resolveSymbols();
    Passes::registerSectionPieces();

    return 0;
}
#include "./util/ArgsParser.h"
#include "./util/ExtractArchive.h"
#include "./util/Singleton.h"
#include "./util/Context.h"
#include "./util/Passes.h"
#include <iostream>
int main(int argc,char **argv){
    std::cerr<<"--------------------"<<std::endl;
    std::cerr<<"---custom ld used---"<<std::endl;
    std::cerr<<"--------------------"<<std::endl;
    std::cerr<<std::endl;
    
    ArgParser::parse(argc,argv);
    UnzipArchiveFiles::unzip();
    PASSES::run(Singleton<Context>());

    return 0;
}
#include "./util/Singleton.h"
#include "./util/ArgsParser.h"
#include "./util/ObjectFile.h"
#include "./util/ArchiveFile.h"
#include <iostream>
int main(int argc,char **argv){
    std::cerr<<"--------------------"<<std::endl;
    std::cerr<<"---custom ld used---"<<std::endl;
    std::cerr<<"--------------------"<<std::endl;
    std::cerr<<std::endl;
    
    ArgParser::parse(argc,argv);
    UnzipArchiveFiles::unzip();
    return 0;
}
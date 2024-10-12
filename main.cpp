#include "./util/ArgsParser.h"
#include "./util/ObjectFile.h"
int main(int argc,char **argv){
    auto AP=ArgsParser(argc,argv);
    auto FileName=AP.getFileName();
    auto OF=ObjectFile::OpenWith(FileName);
    return 0;
}
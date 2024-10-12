#include "ArgsParser.h"

ArgsParser::ArgsParser(int argc,char** argv):Argc(argc),Argv(argv){}

char* ArgsParser::getFileName() const {
    return Argv[1];
}

#pragma once
class ArgsParser{
    int Argc;
    char** Argv;
public:
    ArgsParser(int argc,char** argv);
    char* getFileName() const;
};
#pragma once
#include <string>
#include <vector>
#include "ObjectFile.h"
#include "ArchiveFile.h"
#include "LinkerSymbolTable.h"

struct Context{
    std::string OutputFile="a.out";
    std::vector<std::string> LibraryPaths;
    std::vector<std::string> ArchiveFiles;
    std::vector<std::string> ObjectFiles;

    std::vector<std::unique_ptr<ArchiveFile>> Archives;
    std::vector<std::unique_ptr<ObjectFile>> ExtractObjs;

    std::vector<std::unique_ptr<ObjectFile>> Objs;
    
    LinkerSymbolTable SymbolTable;
};
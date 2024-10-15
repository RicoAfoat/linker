#pragma once
#include <string>
#include <vector>
#include "ObjectFile.h"
#include "ArchiveFile.h"

struct Context{
    std::string OutputFile="a.out";
    std::vector<std::string> LibraryPaths;
    std::vector<std::string> ArchiveFiles;
    std::vector<std::string> ObjectFiles;

    std::vector<std::unique_ptr<ObjectFile>> Objs;
};
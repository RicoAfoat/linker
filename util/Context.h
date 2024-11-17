#pragma once
#include <string>
#include <vector>
#include "ObjectFile.h"
#include "ArchiveFile.h"
#include "LinkerSymbolTable.h"
#include "OutputFile.h"

struct Context{
    std::string OutputFile="a.out";
    std::vector<std::string> LibraryPaths;
    std::vector<std::string> ArchiveFiles;
    std::vector<std::string> ObjectFiles;

    std::vector<std::unique_ptr<ArchiveFile>> Archives;

    LinkerSymbolTable ArchiveSymbolTable;
    LinkerSymbolTable FinalSymbolTable;

    std::queue<std::string> UndefSymbols;
    
    std::vector<std::unique_ptr<ObjectFile>> ExtractObjs;
    std::vector<std::unique_ptr<ObjectFile>> Objs;

    // std::unique_ptr<OutputFile> OutFile;
    std::unique_ptr<OutputObject> OutFile;

    MergedSection* getMergedSection(std::string SectionName,uint32_t ShdrType,uint64_t ShdrFlags);
};
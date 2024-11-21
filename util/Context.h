#pragma once
#include <string>
#include <vector>
#include "ObjectFile.h"
#include "ArchiveFile.h"
#include "LinkerSymbolTable.h"
#include "mergedsection.h"
#include "SectionFragment.h"
#include "mergedsection.h"
#include "mergeablesection.h"
#include "InputSection.h"
#include "OutputEhdr.h"
#include "Symbol.h"
#include "OutputSection.h"
#include "OutputShdr.h"

struct Context{
    std::string OutputFile="a.out";
    std::vector<std::string> LibraryPaths;
    std::vector<std::string> ArchiveFiles;
    std::vector<std::string> ObjectFiles;

    std::vector<std::unique_ptr<ArchiveFile>> Archives;

    std::vector<std::unique_ptr<ObjectFile>> ExtractObjs;
    std::vector<std::unique_ptr<ObjectFile>> Objs;

    LinkerSymbolTable SymbolMap;

    std::vector<std::unique_ptr<MergedSection>> mergedSections;

    // Chunks only for output, not for manage the memory
    std::vector<Chunk*> Chunks;

    std::vector<std::unique_ptr<OutputSection>> OutSections;

    OutputEhdr OutEhdr;
    OutputShdr OutShdr;

    std::vector<uint8_t> OutputBuf;
};
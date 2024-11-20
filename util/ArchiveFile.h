#pragma once
#include "FileBuffer.h"
#include "ELF.h"
#include <memory>

class ArchiveFile:public FileBuffer{
    std::vector<ArHdr*> ArHdrs;
public:
    ArchiveFile()=default;
    void initFileStructure() override;
};

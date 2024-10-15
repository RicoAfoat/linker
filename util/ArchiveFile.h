#pragma once
#include "FileBuffer.h"
#include "ArHeader.h"
#include <memory>

class ArchiveFile:public FileBuffer<ArchiveFile>{
    std::vector<std::unique_ptr<ArHdr>> ArHdrs;
public:
    ArchiveFile()=default;
    void initFileStructure() override;
};

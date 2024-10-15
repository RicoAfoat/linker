#pragma once
#include "ArgsParser.h"
#include "FileBuffer.h"
#include "ArHeader.h"
#include <memory>

namespace UnzipArchiveFiles{
void readArchiveFile(std::string);
void unzip();
} // namespace UnzipArchiveFiles



class ArchiveFile:public FileBuffer<ArchiveFile>{
    std::vector<std::unique_ptr<ArHdr>> ArHdrs;
public:
    ArchiveFile()=default;
    void initFileStructure() override;
};

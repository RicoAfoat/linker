#pragma once
#include "ArgsParser.h"
#include "FileBuffer.h"

namespace UnzipArchiveFiles{
void readArchiveFile(std::string);
void unzip();
} // namespace UnzipArchiveFiles



class ArchiveFile:public FileBuffer<ArchiveFile>{
public:
    ArchiveFile()=default;
    void initFileStructure() override;
};

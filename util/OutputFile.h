#pragma once
#include <string>
#include "RangeAllocator.h"
#include <fstream>
#include <cassert>
#include <map>

class ELFHeader;
class MergedSection;

struct OutputObject{
    std::string FileName;
    RangeAllocator Allocator;
    std::ofstream FileStream;

    std::unique_ptr<ELFHeader> Ehdr;
    std::multimap<std::string, std::unique_ptr<MergedSection>> Sections;
    
    OutputObject(std::string FileName);
    void Write(void* Buffer,size_t Size);
};

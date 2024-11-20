#pragma once
#include "chunk.h"
#include <unordered_map>
#include <string>
#include <memory>
class SectionFragment;
struct MergedSection:public Chunk{
    std::unordered_map<std::string_view,std::unique_ptr<SectionFragment>> Map;
    MergedSection(std::string name,uint64_t flag,uint32_t type);
    static MergedSection* getMergedSection(std::string_view name,uint64_t flag,uint32_t type);
    SectionFragment* Insert(std::string_view Content,uint8_t P2Align);
};

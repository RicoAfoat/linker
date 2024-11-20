#pragma once
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <string>
class MergedSection;
class SectionFragment;
struct MergeableSection {
    MergedSection* Parent;
    uint8_t P2Align=0;
    std::vector<std::string_view> Strs;
    std::vector<uint32_t> FragOffsets;
    std::vector<SectionFragment*> Fragments;
    MergeableSection()=default;
    std::pair<SectionFragment*,uint32_t> getFragment(uint32_t offset);
};


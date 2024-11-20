#pragma once
#include <cstdint>
class MergedSection;
struct SectionFragment{
    MergedSection* OutputSection=nullptr;
    uint32_t Offset=UINT32_MAX;
    uint8_t P2Align=0;
    bool isAlive=false;
    SectionFragment(MergedSection* os);
};

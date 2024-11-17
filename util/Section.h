#pragma once
#include <string>
#include "SectionHeader.h"
#include "RangeAllocator.h"

class ObjectFile;
class Context;
class Section;
class MergedSection;

struct SectionFragment{
    MergedSection* Parent;
    
    void* ContentPtr;
    size_t Size;
    
    uint8_t P2Align;
    bool isAlive;
};

class Section:public SectionHeader{
    void* Content;
public:
    bool isAlive=true;
    Section(ObjectFile* ObjFile,void* ShdrAddr);
    inline std::pair<void*,uint64_t> getContent(){return {Content,getShSize()};}
    std::string getOutputName();
    void splitSection(MergeableSection&);
};



struct MergeableSection{
    MergedSection* Parent=nullptr;
    uint8_t P2Align=0;

    // record its original section offset in order to locate symbol
    std::vector<std::pair<SectionFragment*,uint64_t>> Fragments;

    // use symbol value to locate the fragment
    // return the fragment and the ( offset in the fragment )
    std::pair<SectionFragment*,uint64_t> getFragment(uint64_t Offset);
};

class MergedSection:public SectionHeader{
    std::unordered_map<std::string_view,std::unique_ptr<SectionFragment>> Fragments;
    RangeAllocator Allocator;
public:
    MergedSection():SectionHeader(Allocator(sizeof(Elf64_Shdr))){}

    SectionFragment* insertFragment(void* ptr,size_t Size,uint8_t P2Align,bool isAlive=true);
    // SectionFragment* insertFragment(std::string_view Name,SectionFragment Frag);
    // std::vector<> merge(std::vector<MergeableSectionFragment>&,Section*);
};

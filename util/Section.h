#pragma once
#include "SectionHeader.h"

class ObjectFile;

class Section:public SectionHeader{
    void* Content=nullptr;
public:
    Section(ObjectFile* ObjFile,void* ShdrAddr);
    std::pair<void*,uint64_t> getContent(){return {Content,getShSize()};}
};
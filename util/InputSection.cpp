#include "InputSection.h"
#include "ObjectFile.h"
#include <cassert>

InputSection::InputSection(ObjectFile* f,uint32_t shndx){
    this->File=f;
    this->Shndx=shndx;
    this->Content=f->getBytesFromIdx(shndx);
}

Shdr* InputSection::getShdr(){
    assert(Shndx<File->ElfSections.size());
    return File->ElfSections[Shndx];    
}

std::string_view InputSection::getName(){
    auto shdr=getShdr();
    return ElfGetName(File->Shstrtab,shdr->sh_name);
}

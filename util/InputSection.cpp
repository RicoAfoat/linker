#include "InputSection.h"
#include "ObjectFile.h"
#include "OutputSection.h"
#include <cassert>

InputSection::InputSection(ObjectFile* f,uint32_t shndx){
    this->File=f;
    this->Shndx=shndx;
    this->Content=f->getBytesFromIdx(shndx);
    this->isAlive=true;

    auto shdr=getShdr();
    assert((shdr->sh_flags&SHF_COMPRESSED)==0);

    auto toP2Align=[](uint64_t alignment) ->uint8_t {
        for(uint8_t i=0;i<64;i++)
            if(alignment&((uint64_t)1<<i))
                return i;
        return 0;
    };

    this->P2Align=toP2Align(shdr->sh_addralign);
    // this->OutSec=OutputSection::getOutputSection(std::string(this->getName()),shdr->sh_type,shdr->sh_flags);
    // this->OutSec->Member.push_back(this);
}

Shdr* InputSection::getShdr(){
    assert(Shndx<File->ElfSections.size());
    return File->ElfSections[Shndx];    
}

std::string_view InputSection::getName(){
    auto shdr=getShdr();
    return ElfGetName(File->Shstrtab,shdr->sh_name);
}

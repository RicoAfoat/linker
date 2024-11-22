#include "InputFile.h"
#include "Symbol.h"
#include <cassert>

void InputFile::initFileStructure(){
    auto [Content,Limi]=getFileBuffer();

    auto MoveForward=[&Content,&Limi](auto size){
        Content+=size;
        Limi-=size;
    };

    auto ehdr=(Ehdr*)Content;MoveForward(ehdr->e_shoff);
    
    auto shdr=(Shdr*)Content;
    
    uint64_t numSections=ehdr->e_shnum;
    if(numSections==0)numSections=shdr->sh_size;

    ElfSections.push_back(shdr);
    for(auto i=1;i<numSections;i++){
        MoveForward(sizeof(Shdr));
        shdr=(Shdr*)Content;
        ElfSections.push_back(shdr);
    }

    uint64_t shstrndx;
    if(ehdr->e_shstrndx==SHN_XINDEX)
        shstrndx=shdr->sh_link;
    else
        shstrndx=ehdr->e_shstrndx;
    
    this->Shstrtab=getBytesFromIdx(shstrndx);
}

std::pair<uint8_t*,size_t> InputFile::getBytesFromShdr(Shdr* s){
    auto end=s->sh_offset+s->sh_size;
    assert(end<=getFileBuffer().second&&"Out of Range");
    return {getFileBuffer().first+s->sh_offset,s->sh_size};
}

std::pair<uint8_t*,size_t> InputFile::getBytesFromIdx(int64_t Idx){
    assert(Idx<ElfSections.size());
    return getBytesFromShdr(ElfSections[Idx]);
}

void  InputFile::fillupElfSyms(Shdr* s){
    auto [Content,Limi]=getBytesFromShdr(s);
    auto MoveForward=[&Content,&Limi](auto size){
        Content+=size;
        Limi-=size;
    };
    auto nums=Limi/sizeof(Sym);
    for(auto i=0;i<nums;i++){
        ElfSyms.push_back((Sym*)Content);
        MoveForward(sizeof(Sym));
    }
}

Shdr* InputFile::findSection(uint32_t type){
    for(auto s:ElfSections)
        if(s->sh_type==type)return s;
    return nullptr;
}

Ehdr* InputFile::getEhdr(){
    return (Ehdr*)getFileBuffer().first;
}

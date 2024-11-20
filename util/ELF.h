#pragma once

#include <string>
#include <elf.h>
#include <ar.h>

using Ehdr=Elf64_Ehdr;
// using Phdr=Elf64_Phdr;
using Shdr=Elf64_Shdr;
using Sym=Elf64_Sym;
using ArHdr=ar_hdr;

bool IsAbs(Sym*);
bool IsUndef(Sym*);
bool IsCommon(Sym*);

std::string_view ElfGetName(std::pair<uint8_t*,size_t> StrTab,uint32_t Offset);

uint64_t getArSectionSize(ArHdr* hdr);

enum class ArEnumtype : uint8_t {
    AR_SYM,
    AR_STR,
    AR_OBJ
};

ArEnumtype getArEnumtype(ArHdr* hdr);

std::string getObjfileName(ArHdr* hdr,ArHdr* Strtab);

std::pair<uint8_t*,size_t> getArSection(ArHdr* hdr);

void ZeroInit(void* ptr,size_t size);

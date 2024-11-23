#pragma once

#include <string>
#include <elf.h>
#include <ar.h>

using Ehdr=Elf64_Ehdr;
// using Phdr=Elf64_Phdr;
using Shdr=Elf64_Shdr;
using Sym=Elf64_Sym;
using ArHdr=ar_hdr;
using Phdr=Elf64_Phdr;

using Rela=Elf64_Rela;
// struct Rela{
//     uint64_t Offset;
//     uint32_t Type;
//     uint32_t Sym;
//     int64_t Addend;
// };

const uint64_t IMAGE_BASE=0x200000;

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

uint64_t AlignTo(uint64_t val,uint64_t align);

void WriteMagic(void* dst);

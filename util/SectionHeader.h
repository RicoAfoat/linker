#pragma once
#define GET_SectionHeaderLayOut
#include "DataLayoutAdapter.h"

enum class SH_Type_Enum: uint32_t {
  SHT_NULL = 0x0,
  SHT_PROGBITS = 0x1,
  SHT_SYMTAB = 0x2,
  SHT_STRTAB = 0x3,
  SHT_RELA = 0x4,
  SHT_HASH = 0x5,
  SHT_DYNAMIC = 0x6,
  SHT_NOTE = 0x7,
  SHT_NOBITS = 0x8,
  SHT_REL = 0x9,
  SHT_SHLIB = 0x0A,
  SHT_DYNSYM = 0x0B,
  SHT_INIT_ARRAY = 0x0E,
  SHT_FINI_ARRAY = 0x0F,
  SHT_PREINIT_ARRAY = 0x10,
  SHT_GROUP = 0x11,
  SHT_SYMTAB_SHNDX = 0x12,
  SHT_NUM = 0x13,
  SHT_LOOS = 0x60000000,
};

constexpr const char SH_NAME[] = "sh_name";
class SectionHeader : public SystemStructAdapter,
                      public AttributeGetNameOffset<SectionHeader,SH_NAME>,
                      public AttributeStringNameOffset<SectionHeader> {
public:
  SectionHeader(
      void *StartAddr, size_t Size,
      std::initializer_list<std::tuple<std::string, size_t, uint8_t>> initList)
      : SystemStructAdapter(StartAddr, Size, initList){};

  // inline uint32_t getNameOffset() {
  //   return loadComponentAs<uint32_t>("sh_name");
  // }

  inline uint32_t getShInfo(){
    return loadComponentAs<uint32_t>("sh_info");
  }

  inline uint32_t getShLink() { return loadComponentAs<uint32_t>("sh_link"); }

  inline uint64_t getShSize() { return loadComponentAs<uint64_t>("sh_size"); }

  inline uint32_t getShType() { return loadComponentAs<uint32_t>("sh_type"); }

  inline uint64_t getSectionOffset() {
    return loadComponentAs<uint64_t>("sh_offset");
  }
};
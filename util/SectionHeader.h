#pragma once
#include "DataLayoutAdapter.h"
#include <elf.h>

class SectionHeader : public SystemStructAdapter,
                      public AttributeStringNameOffset<SectionHeader,ConstTableEntry::sh_name> {
public:
  SectionHeader(
      void *StartAddr)
      : SystemStructAdapter(StartAddr, ConstTableEntry::sh_name,ConstTableEntry::EndOfSectionHeader){};

  // inline uint32_t getNameOffset() {
  //   return loadComponentAs<uint32_t>("sh_name");
  // }

  inline uint32_t getShInfo(){
    return loadComponentAs<uint32_t>(ConstTableEntry::sh_info);
  }

  inline uint32_t getShLink() { return loadComponentAs<uint32_t>(ConstTableEntry::sh_link); }

  inline uint64_t getShSize() { return loadComponentAs<uint64_t>(ConstTableEntry::sh_size); }

  inline uint32_t getShType() { return loadComponentAs<uint32_t>(ConstTableEntry::sh_type); }

  inline uint64_t getSectionOffset() {
    return loadComponentAs<uint64_t>(ConstTableEntry::sh_offset);
  }
};

extern template SectionHeader *getNew(void *, uint32_t);

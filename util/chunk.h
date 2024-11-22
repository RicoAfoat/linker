#pragma once
#include <cstddef>
#include <cstdint>
#include "ELF.h"

struct Chunk {
protected:
    Shdr SectionHeader;
    uint64_t Shndx=0;
    std::string Name;
public:
    void CopyInto(uint8_t* buf, size_t size);
    Chunk();
    uint32_t rank();
    virtual Shdr* getShdr();
    virtual void CopyBuf();
    virtual void UpdateShdr();
    virtual uint64_t getShndx();
    virtual std::string getName();
    virtual ~Chunk()=default;
};



/* global offset table */
#pragma once

#include "chunk.h"
#include "Symbol.h"
#include <vector>

struct GotEntry{
    int64_t Idx=0;
    uint64_t Val=0;
};

struct GotSection:public Chunk{
    std::vector<Symbol*> GotTpSyms;
    GotSection();
    void addGotTpSym(Symbol* sym);
    void CopyBuf() override;
    std::vector<GotEntry> getEntries();
};


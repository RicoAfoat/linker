#define GET_Elf_Sym
#include "ELFSym.h"

template ELFSym* getNewImpl<ELFSym>(DataLayOutEnum, void*, size_t);


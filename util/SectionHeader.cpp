#define GET_SectionHeaderLayOut
#include "SectionHeader.h"
#include <cassert>

template SectionHeader* getNewImpl<SectionHeader>(DataLayOutEnum, void*, size_t);

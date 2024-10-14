#include "SectionHeader.h"
#include <cassert>

template SectionHeader* getNew<SectionHeader,DataLayOutEnum, void*, size_t>(DataLayOutEnum&&, void*&&, size_t&&);

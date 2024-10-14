#define GET_ARHDR
#include "ArHeader.h"

template ArHdr* getNew<ArHdr, void*, size_t>(void*&&, size_t&&);
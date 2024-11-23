#include "SectionFragment.h"
#include "mergedsection.h"

SectionFragment::SectionFragment(MergedSection* os):OutputSection(os){}

uint64_t SectionFragment::getAddr(){
    return OutputSection->getShdr()->sh_addr+Offset;
}

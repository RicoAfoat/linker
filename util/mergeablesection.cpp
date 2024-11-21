#include "mergeablesection.h"
#include "SectionFragment.h"
#include "mergedsection.h"

std::pair<SectionFragment*,uint32_t> MergeableSection::getFragment(uint32_t offset){
    auto pos=std::upper_bound(FragOffsets.begin(),FragOffsets.end(),offset);
    
    auto idx=std::distance(FragOffsets.begin(),pos)-1;
    return std::make_pair(Fragments[idx],offset-FragOffsets[idx]);
}

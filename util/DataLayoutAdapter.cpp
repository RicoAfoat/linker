#include "DataLayoutAdapter.h"

bool SystemStructAdapter::check(){return true;}

SystemStructAdapter::SystemStructAdapter(void* _StartAddr,ConstTableEntry _begin,ConstTableEntry _end){
    StartAddr=_StartAddr;
    Begin=_begin;
    End=_end;
    Size=getConstInfo[End].Offset;
}

std::string SystemStructAdapter::loadRAW(ConstTableEntry Entry){
    auto [Offset,EleSize]=getConstInfo[Entry];
    std::string Ret((char*)StartAddr+Offset,EleSize);
    return Ret;
}
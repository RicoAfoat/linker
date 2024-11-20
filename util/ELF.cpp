#include "ELF.h"
#include <cassert>
#include "Stringlib.h"

void ZeroInit(void* ptr,size_t size){
    for(size_t i=0;i<size;i++)
        ((uint8_t*)ptr)[i]=0;
}

std::string getObjfileName(ArHdr* hdr,ArHdr* AHStrtab){
    auto RawName=std::string(hdr->ar_name,sizeof(hdr->ar_name));
    // Long filename
    if(hasPrefix(RawName,"/")){
        uint32_t Offset=std::stoul(trimSpace(RawName.substr(1)));
        auto findEndOffset=[](char* str){
            bool findPH=false;
            uint32_t Res=0;
            for(uint32_t i=0;;i++){
                if(str[i]=='/'){
                    Res=i;
                    findPH=true;
                    continue;
                }
                if(str[i]=='\n')
                    if(findPH==true)
                        return Res;
                findPH=false;
                Res=0;
            }
        };
        auto [Strtab,StrtabSize]=getArSection(AHStrtab);
        uint32_t SuffixOffset=Offset+findEndOffset((char*)Strtab+Offset);
        // [Offset,SuffixOffset)
        return std::string((char*)Strtab+Offset,(char*)Strtab+SuffixOffset);
    }

    // Short filename
    auto end=RawName.find_first_of("/");
    assert(end!=std::string::npos);
    return RawName.substr(0,end);
}

std::string_view ElfGetName(std::pair<uint8_t*,size_t> StrTab,uint32_t Offset){
    for(auto i=StrTab.first+Offset;i<StrTab.first+StrTab.second;i++){
        if(*i==0){
            return std::string_view((char*)StrTab.first+Offset,i-(StrTab.first+Offset));
        }
    }
    assert(0&&"unreachable");
}

uint64_t getArSectionSize(ArHdr* hdr){
    return std::stoul(trimSpace(std::string(hdr->ar_size,sizeof(hdr->ar_size))));
}

ArEnumtype getArEnumtype(ArHdr* hdr){
    auto RawName=std::string(hdr->ar_name,sizeof(hdr->ar_name));
    if(hasPrefix(RawName,"// "))
        return ArEnumtype::AR_STR;
    else if(hasPrefix(RawName,"/ ")||hasPrefix(RawName,"/SYM64/ "))
        return ArEnumtype::AR_SYM;
    else return ArEnumtype::AR_OBJ;
}

bool IsAbs(Sym* sym){
    return sym->st_shndx==SHN_ABS;
}

bool IsUndef(Sym* sym){
    return sym->st_shndx==SHN_UNDEF;
}

bool IsCommon(Sym* sym){
    return sym->st_shndx==SHN_COMMON;
}

std::pair<uint8_t*,size_t> getArSection(ArHdr* hdr){
    return {(uint8_t*)(hdr)+sizeof(ArHdr),getArSectionSize(hdr)};
}

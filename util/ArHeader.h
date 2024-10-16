#pragma once
#include "DataLayoutAdapter.h"
#include <algorithm>
#include <cctype>
#include <cstring>

static inline std::string trimSpace(std::string str){
    auto start = std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    auto end = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base();
    return std::string(start, end);
}

static inline bool hasPrefix(const std::string& fullString, const std::string& starting) {
    if (fullString.find(starting) == 0)
        return true;
    return false;
}

enum class ArEnumtype : uint8_t {
    AR_SYM,
    AR_STR,
    AR_OBJ
};

class ArHdr:public SystemStructAdapter{
    std::string ObjFileName;
public:
    ArHdr(void *StartAddr) : SystemStructAdapter(StartAddr, ConstTableEntry::ar_name, ConstTableEntry::EndOfArHdr){};
    inline void* getArSectionAddr(){
        return (uint8_t*)getStartAddr()+getSize();
    }
    inline uint32_t getArSectionSize(){
        std::string ArSectionSize=trimSpace(loadRAW(ConstTableEntry::ar_size));
        return std::stoul(ArSectionSize,nullptr,10);
    }
    inline ArEnumtype getType(){
        std::string Name=loadRAW(ConstTableEntry::ar_name);
        if(hasPrefix(Name,"// "))
            return ArEnumtype::AR_STR;
        else if(hasPrefix(Name,"/ ")||hasPrefix(Name,"/SYM64/ "))
            return ArEnumtype::AR_SYM;
        else return ArEnumtype::AR_OBJ;
    }
    inline std::string getObjfileName(ArHdr* Strtab){
        assert(getType()==ArEnumtype::AR_OBJ);
        if(!ObjFileName.empty())return ObjFileName;
        std::string FileName=loadRAW(ConstTableEntry::ar_name);
        // long file name
        if(hasPrefix(FileName,"/")){
            uint32_t Offset=std::stoul(trimSpace(FileName.substr(1)));
            // end with /\n
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
            char* StrtabAddr=(char*)Strtab->getArSectionAddr();
            uint32_t SuffixOffset=Offset+findEndOffset(StrtabAddr+Offset);
            // [Offset,SuffixOffset)
            ObjFileName=std::string(StrtabAddr+Offset,StrtabAddr+SuffixOffset);
        }
        // short file name
        else
            ObjFileName=std::string(FileName.begin(),std::find_if(FileName.begin(), FileName.end(), [](unsigned char ch) {return ch=='/';}));
        return ObjFileName;
    }
};

extern template ArHdr* getNew(void*,uint32_t);
#pragma once
#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <string>
#include <cassert>
#include <iostream>

enum class DataLayOutEnum : uint8_t {
    BIT64,
    BIT32
};

struct ConstTable{
    uint32_t Offset;
    uint32_t Size;
};

enum ConstTableEntry{
#define GET_CONST_TABLE_ENTRY
#include "../conf/RISCV64.def"
};

const ConstTable getConstInfo[]{
#define GET_CONST_TABLE
#include "../conf/RISCV64.def"
};

class SystemStructAdapter{
    ConstTableEntry Begin,End;
    inline void checkEntry(ConstTableEntry Entry){assert(Entry>=Begin&&Entry<=End&&"Entry out of range");}
protected:
    void* StartAddr;
    size_t Size;
public:
    virtual bool check();
    inline void* getStartAddr(){return StartAddr;}
    
    inline size_t getSize(){return Size;}

    template<typename T>
    T* getComponent(ConstTableEntry Entry){
        checkEntry(Entry);
        auto& [Offset,EleSize]=getConstInfo[Entry];
        void* Addr=(uint8_t*)StartAddr+Offset;
        return (T*)Addr;
    }
    template<typename T>
    T loadComponentAs(ConstTableEntry Entry){
        checkEntry(Entry);
        auto& [Offset,EleSize]=getConstInfo[Entry];
        void* Addr=(uint8_t*)StartAddr+Offset;
        switch (EleSize)
        {
        default:
            assert(0&&"unreachable");
            break;
        case 1:
            assert(sizeof(T)>=1);
            return (T)*(uint8_t*)(Addr);
        case 2:
            assert(sizeof(T)>=2);
            return (T)*(uint16_t*)(Addr);
        case 4:
            assert(sizeof(T)>=4);
            return (T)*(uint32_t*)(Addr);
        case 8:
            assert(sizeof(T)>=8);
            return (T)*(uint64_t*)(Addr);
        }
    }

    std::string loadRAW(ConstTableEntry);
    
    SystemStructAdapter(void* _StartAddr, ConstTableEntry _begin, ConstTableEntry _end);
};

class Attribute{
public:
    virtual ~Attribute()=default;
};

template<typename T,ConstTableEntry Name>
class AttributeStringNameOffset:public Attribute{
    char* NamePtr=nullptr;
public:
    AttributeStringNameOffset()=default;
    uint32_t getNameOffset(){
        auto derived=dynamic_cast<T*>(this);
        return derived->template loadComponentAs<uint32_t>(Name);
    }
    void dumpName(){
        assert(NamePtr!=nullptr&&"initAttributeStringNameOffset not called");
        std::printf("%s\n",NamePtr);
    }
    std::string getName(){
        assert(NamePtr!=nullptr&&"initAttributeStringNameOffset not called");
        return std::string(NamePtr);
    }
    void initAttributeStringNameOffset(void* NameSectionAddr){
        auto derived=dynamic_cast<T*>(this);
        auto NameOffset=derived->getNameOffset();
        NamePtr=(char*)NameSectionAddr+NameOffset;
        // dumpName();
    }
};

template<typename T>
T* getNew(void* StartAddr,uint32_t RestSize){
    T* Storage=new T(StartAddr);
    assert(Storage->getSize()<=RestSize&&"file does not contain enough space");
    return Storage;
}

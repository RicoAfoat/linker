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

class SystemStructAdapter{
    std::unordered_map<std::string,std::tuple<void*,uint8_t>> ComponentMap;
protected:
    void* StartAddr;
    size_t Size;
public:
    virtual bool check();
    inline void* getStartAddr(){return StartAddr;}
    
    inline size_t getSize(){return Size;}

    template<typename T>
    T* getComponent(std::string Name){
        auto& [Addr,EleSize]=ComponentMap[Name];
        return (T*)Addr;
    }
    template<typename T>
    T loadComponentAs(std::string Name){
        auto& [Addr,EleSize]=ComponentMap[Name];
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

    std::string loadRAW(std::string Name);
    
    SystemStructAdapter(void* _StartAddr, size_t _Size,std::initializer_list<std::tuple<std::string, size_t, uint8_t>> initList);
};

class Attribute{
public:
    virtual ~Attribute()=default;
};

template<typename T>
class AttributeStringNameOffset:public Attribute{
    char* NamePtr=nullptr;
public:
    AttributeStringNameOffset()=default;
    void dumpName(){
        assert(NamePtr!=nullptr&&"initAttributeStringNameOffset not called");
        std::printf("%s\n",NamePtr);
    }
    void initAttributeStringNameOffset(void* NameSectionAddr){
        auto derived=dynamic_cast<T*>(this);
        auto NameOffset=derived->getNameOffset();
        NamePtr=(char*)NameSectionAddr+NameOffset;
        // dumpName();
    }
};

template <typename T,const char* Name>
class AttributeGetNameOffset:public Attribute{
public:
    uint32_t getNameOffset(){
        auto derived=dynamic_cast<T*>(this);
        return derived->template loadComponentAs<uint32_t>(Name);
    }
};

template<typename T>
T* getNewImpl(DataLayOutEnum ChoosedLayout,void* StartAddr,size_t Size){
    T* Storage=nullptr;
    if(ChoosedLayout==DataLayOutEnum::BIT64){
        Storage=new T(
            StartAddr,
#include "../conf/RISCV64.def"
        );
    }
    else
        assert(0&&"unimpl");
    assert(Size>=Storage->getSize()&&"file does not contain enough space");
    assert(Storage->check()&&"self check failed");
    return Storage;
}

template<typename T>
T* getNewImpl(void* StartAddr,size_t Size){
    T* Storage=new T(
        StartAddr,
#include "../conf/Universal.def"
    );
    return Storage;
}

template<typename T,typename... Args>
T* getNew(Args&&... args){
    return getNewImpl<T>(std::forward<Args>(args)...);
}

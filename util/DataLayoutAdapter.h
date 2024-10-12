#pragma once
#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <string>
#include <cassert>

enum DataLayOutEnum{
    BIT64,
    BIT32
};

class SystemStructAdapter{;
    std::unordered_map<std::string,std::tuple<void*,uint8_t>> ComponentMap;
protected:
    void* StartAddr;
    size_t Size;
public:
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
    SystemStructAdapter(void* _StartAddr, size_t _Size,std::initializer_list<std::tuple<std::string, size_t, uint8_t>> initList);
};
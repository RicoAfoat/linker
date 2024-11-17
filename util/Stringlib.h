#pragma once
#include <string>
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

static bool AllZero(char* ptr,uint32_t size){
    for(uint32_t i=0;i<size;i++)
        if(ptr[i]!=0)
            return false;
    return true;
}

static inline uint64_t findNullOffset(char* ptr,uint32_t strideAlign){
    for(uint64_t i=0;;i+=strideAlign){
        if(AllZero(ptr+i,strideAlign))
            return i;
    }
    // unreachable
}
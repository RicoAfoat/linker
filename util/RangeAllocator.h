#pragma once
#include <vector>
#include <memory>
class RangeAllocator{
    std::vector<std::unique_ptr<void,void(*)(void*)>> ptrs;
    std::vector<void*> Mallocs;
public:
    template <typename T,typename... Args>
    T* emplace(Args&&... args){
        auto ptr=new T(std::forward<Args>(args)...);
        ptrs.push_back(ptr,std::default_delete<T>());
        return ptr;
    }

    void* operator()(size_t size){
        auto ptr=malloc(size);
        for(int i=0;i<size;i++)
            ((char*)ptr)[i]=0;
        Mallocs.push_back(ptr);
        return ptr;
    }

    ~RangeAllocator(){
        for(auto ptr:Mallocs)
            free(ptr);
    }
};
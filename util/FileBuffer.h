#pragma once
#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>

// A very slow one that will read all the contents in the file to a binary buffer
template<typename T>
class FileBuffer{
protected:
    std::vector<uint8_t> FileStorage;
    std::pair<uint8_t*,uint32_t> FileRef;
private:
    std::string FileName;
    std::vector<uint8_t>& getFileStorage(){return FileStorage;}
public:
    FileBuffer()=default;
    virtual ~FileBuffer()=default;
    virtual void initFileStructure()=0;
    
    std::pair<uint8_t*,uint32_t> getFileBuffer(){return FileRef;}

    static T* OpenWith(std::string FileName){
        std::ifstream file(FileName, std::ios::binary | std::ios::ate);
        if (!file.is_open())
            return nullptr;

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        auto OF =new T();
        auto& buffer = OF->getFileStorage();
        buffer=std::vector<uint8_t>(size);
        file.read((char*)buffer.data(), size);
        file.close();

        OF->FileRef=std::make_pair(buffer.data(),buffer.size());
        OF->FileName=FileName;
        OF->initFileStructure();
        
        return OF;
    }
    // used for those extract from archive files
    static T* OpenWith(std::string FileName,uint8_t* Addr,uint32_t Size){
        auto OF=new T();
        OF->FileRef=std::make_pair(Addr,Size);
        OF->FileName=FileName;
        OF->initFileStructure();
        return OF;
    }
};

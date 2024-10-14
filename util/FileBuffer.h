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
public:
    FileBuffer()=default;
    virtual ~FileBuffer()=default;
    virtual void initFileStructure()=0;
    std::vector<uint8_t>& getFileStorage(){return FileStorage;}

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

        OF->initFileStructure();
        
        return OF;
    }
};

#include "OutputFile.h"

OutputObject::OutputObject(std::string FileName):FileName(FileName){
    FileStream.open(FileName,std::ios::binary);
    assert(FileStream.is_open()&&"Open file failed");
}

void OutputObject::Write(void* Buffer,size_t Size){
    FileStream.write((char*)Buffer,Size);
}

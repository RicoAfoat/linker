#include "Singleton.h"
#include "ArchiveFile.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstring>

void ArchiveFile::initFileStructure(){
    // check file header
    // !<arch>\n
    auto StartAddr=getFileStorage().data();
    auto Limi=getFileStorage().size();
    assert(strncmp((const char*)StartAddr,"!<arch>\n",8)==0);

    // !<arch> [ArHdr][body]
    for(auto Offset=8;Offset<Limi;){
        auto CurAddr=StartAddr+Offset;
        
        if(Offset%2==1)Offset++;
    }
}

void UnzipArchiveFiles::readArchiveFile(std::string fileName){
    auto& IncludeDir=Singleton<Context>().LibraryPaths;

    for(auto& Dir:IncludeDir){
        // construct the path
        std::string path=Dir+"/"+fileName;
        std::cerr<<"Trying to open "<<path<<std::endl;
        
        auto ArchiveFile=ArchiveFile::OpenWith(path);
        if(ArchiveFile==nullptr)continue;

        delete ArchiveFile;break;
    }

    assert(0&&"Cannot find the file");
}

void UnzipArchiveFiles::unzip(){
    auto& Ctx=Singleton<Context>();
    for(auto& ArchiveFile:Ctx.ArchiveFiles){
        std::cerr<<"Unzipping "<<ArchiveFile<<std::endl;
        readArchiveFile(ArchiveFile);
    }
}
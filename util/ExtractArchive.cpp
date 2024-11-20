#include "ExtractArchive.h"
#include "Singleton.h"
#include "Context.h"
#include "InputSection.h"
#include <cassert>

void UnzipArchiveFiles::readArchiveFile(std::string fileName){
    auto& IncludeDir=Singleton<Context>().LibraryPaths;

    for(auto& Dir:IncludeDir){
        // construct the path
        std::string path=Dir+"/"+fileName;
        std::cerr<<"Trying to open "<<path<<std::endl;
        
        auto AF=FileBuffer::OpenWith<ArchiveFile>(path);
        if(AF==nullptr)continue;
        Singleton<Context>().Archives.emplace_back(AF);
        return;
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
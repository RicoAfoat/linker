#include "Singleton.h"
#include "ArchiveFile.h"
#include "ArHeader.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstring>

void ArchiveFile::initFileStructure(){
    // check file header
    // !<arch>\n
    auto [StartAddr,Limi]=getFileBuffer();
    assert(strncmp((const char*)StartAddr,"!<arch>\n",8)==0);

    // !<arch>\n [ArHdr][body]
    ArHdr* Strtab=nullptr;
    for(auto Offset=8;Offset<Limi;){
        auto CurAddr=StartAddr+Offset;
        auto Arhdr=getNew<ArHdr>(CurAddr,Limi-Offset);
        Offset+=Arhdr->getSize()/*ArHdr size*/+Arhdr->getArSectionSize()/*Following Ar Section's Size*/;
        
        switch (Arhdr->getType())
        {
        case ArEnumtype::AR_STR:
            Strtab=Arhdr;
            break;
        case ArEnumtype::AR_SYM:
            delete Arhdr;
            break;
        case ArEnumtype::AR_OBJ:
            ArHdrs.emplace_back(Arhdr);
            break;
        default:
            assert(0&&"unimpl");
        }
        if(Offset%2==1)Offset++;
    }
    
    // use Strtab to read all Obj names(if there're long name)
    for(auto& hdr:ArHdrs)
        std::cerr<<hdr->getObjfileName(Strtab)<<std::endl;

    delete Strtab;
}

void UnzipArchiveFiles::readArchiveFile(std::string fileName){
    auto& IncludeDir=Singleton<Context>().LibraryPaths;

    for(auto& Dir:IncludeDir){
        // construct the path
        std::string path=Dir+"/"+fileName;
        std::cerr<<"Trying to open "<<path<<std::endl;
        
        auto ArchiveFile=ArchiveFile::OpenWith(path);
        if(ArchiveFile==nullptr)continue;

        delete ArchiveFile;return;
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
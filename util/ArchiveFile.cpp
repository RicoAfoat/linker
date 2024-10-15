#include "Context.h"
#include "Singleton.h"

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
        auto Arhdr=getNewImpl<ArHdr>(CurAddr,Limi-Offset);
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
    for(auto& hdr:ArHdrs){
        auto Name=hdr->getObjfileName(Strtab);
        auto ObjAddr=hdr->getArSectionAddr();
        auto ObjSize=hdr->getArSectionSize();
        std::cerr<<"Extracting Object File:"<<Name<<"\n";
        Singleton<Context>().Objs.emplace_back(ObjectFile::OpenWith(Name,(uint8_t*)ObjAddr,ObjSize));
    }
    
    delete Strtab;
}

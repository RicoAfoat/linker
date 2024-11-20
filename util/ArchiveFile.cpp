#include "Context.h"
#include "Singleton.h"
#include "ArchiveFile.h"
#include "InputSection.h"
#include "Symbol.h"
#include "SectionFragment.h"
#include "mergedsection.h"
#include "mergeablesection.h"

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
        
        auto AH=(ArHdr*)CurAddr;
        Offset+=sizeof(ArHdr)+getArSectionSize(AH);

        switch (getArEnumtype(AH))
        {
        case ArEnumtype::AR_STR:
            Strtab=AH;
            break;
        case ArEnumtype::AR_SYM:
            /* 后面可以用这个来加速解析，只考虑功能性是非必要的 */
            break;
        case ArEnumtype::AR_OBJ:
            ArHdrs.emplace_back(AH);
            break;
        default:
            assert(0&&"unimpl");
        }

        if(Offset%2==1)Offset++;
    }
    
    // use Strtab to read all Obj names(if there're long name)
    for(auto& hdr:ArHdrs){
        auto Name=getObjfileName(hdr,Strtab);
        auto [ObjAddr,ObjSize]=getArSection(hdr);
        std::cerr<<"Extracting Object File:"<<Name<<"\n";
        auto Obj=FileBuffer::OpenWith<ObjectFile>(Name,(uint8_t*)ObjAddr,ObjSize);
        Obj->isAlive=false;
        // Obj->setArchiveFile(this);
        Singleton<Context>().ExtractObjs.emplace_back(Obj);
    }
}

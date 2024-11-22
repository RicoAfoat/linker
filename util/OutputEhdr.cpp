#include "Singleton.h"
#include "Context.h"
#include <cstring>
#include <cassert>

OutputEhdr::OutputEhdr(){
    auto shdr=getShdr();
    shdr->sh_flags=SHF_ALLOC;
    shdr->sh_size=sizeof(Ehdr);
    shdr->sh_addralign=8;
}

void OutputEhdr::CopyBuf(){
    auto& Ctx=Singleton<Context>();
    
    auto ehdr=Ehdr();ZeroInit(&ehdr,sizeof(Ehdr));
    WriteMagic(ehdr.e_ident);
    ehdr.e_ident[EI_CLASS]=ELFCLASS64;
    ehdr.e_ident[EI_DATA]=ELFDATA2LSB;
    ehdr.e_ident[EI_VERSION]=EV_CURRENT;
    ehdr.e_ident[EI_OSABI]=ELFOSABI_NONE;
    ehdr.e_ident[EI_ABIVERSION]=0;
    ehdr.e_type=ET_EXEC;
    ehdr.e_machine=EM_RISCV;
    ehdr.e_version=EV_CURRENT;
    ehdr.e_entry=getEntryAddr();
    ehdr.e_phoff=Ctx.OutPhdr.getShdr()->sh_offset;
    ehdr.e_shoff=Ctx.OutShdr.getShdr()->sh_offset;
    ehdr.e_ehsize=sizeof(Ehdr);
    ehdr.e_phentsize=sizeof(Phdr);
    ehdr.e_phnum=uint16_t(Ctx.OutPhdr.getShdr()->sh_size/sizeof(Phdr));
    ehdr.e_shentsize=sizeof(Shdr);
    ehdr.e_shnum=uint16_t(Ctx.OutShdr.getShdr()->sh_size/sizeof(Shdr));

    // copy to the output stream
    CopyInto((uint8_t*)&ehdr,sizeof(Ehdr));
}

uint64_t OutputEhdr::getEntryAddr(){
    auto& OSecs=Singleton<Context>().OutSections;
    for(auto& osec:OSecs)
        if(osec->getName()==".text")
            return osec->getShdr()->sh_addr;
    return 0;
}

// RVC is a compressed instruction set for RISC-V, set the flag
uint32_t OutputEhdr::getFlags(){
    auto& Ctx=Singleton<Context>();
    assert(Ctx.Objs.size()>0);
    auto flag=Ctx.Objs[0]->getEhdr()->e_flags;
    for(decltype(Ctx.Objs.size()) i=1,limi=Ctx.Objs.size();i<limi;i++){
        auto& obj=Ctx.Objs[i];
        if(obj->getEhdr()->e_flags&EF_RISCV_RVC){
            flag|=EF_RISCV_RVC;
            break;
        }
    }

    return flag;
}

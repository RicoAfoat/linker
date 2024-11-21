#include "Singleton.h"
#include "Context.h"
#include <cstring>

OutputEhdr::OutputEhdr(){
    auto shdr=getShdr();
    shdr->sh_flags=SHF_ALLOC;
    shdr->sh_size=sizeof(Ehdr);
    shdr->sh_addr=8;
}

void OutputEhdr::CopyBuf(){
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
    //
    ehdr.e_ehsize=sizeof(Ehdr);
    ehdr.e_phentsize=sizeof(Phdr);
    //
    ehdr.e_shentsize=sizeof(Shdr);
    //

    // copy to the output stream
    auto& Ctx=Singleton<Context>();
    auto copylocate=Ctx.OutputBuf.data()+getShdr()->sh_offset;
    memcpy(copylocate,&ehdr,sizeof(Ehdr));
}

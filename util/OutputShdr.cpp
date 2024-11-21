#include "OutputShdr.h"
#include "Singleton.h"
#include "Context.h"
#include <cstring>

OutputShdr::OutputShdr(){
    SectionHeader.sh_addralign=8;
}

void OutputShdr::UpdateShdr(){
    uint64_t n=0;
    
    auto& Ctx=Singleton<Context>();
    for(auto chunk:Ctx.Chunks)
        if(chunk->getShndx()>0)
            n=chunk->getShndx();
    
    SectionHeader.sh_size=(n+1)*sizeof(Shdr);
}

void OutputShdr::CopyBuf(){
    auto& Ctx=Singleton<Context>();
    auto copylocate=Ctx.OutputBuf.data()+SectionHeader.sh_offset;
    memcpy(copylocate,&SectionHeader,sizeof(Shdr));
    for(auto chunk:Ctx.Chunks)
        if(chunk->getShndx()>0)
            memcpy(copylocate+chunk->getShndx()*sizeof(Shdr),chunk->getShdr(),sizeof(Shdr));
}

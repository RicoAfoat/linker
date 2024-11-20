#include "Output.h"
#include "ELF.h"
#include "Stringlib.h"

static const char* prefix[]={
    ".text.",
    ".data.rel.ro.",
    ".data.",
    ".rodata.",
    ".bss.rel.ro.",
    ".bss.",
    ".init_array.",
    ".fini_array.",
    ".tbss.",
    ".tdata.",
    ".gcc_except_table.",
    ".ctors.",
    ".dtors.",
};


std::string getOutputName(std::string Name,uint64_t flag){
    if(Name==".rodata"||hasPrefix(Name,".rodata.")&&flag&SHF_MERGE!=0){
        if(flag&SHF_STRINGS)
            return ".rodata.str";
        else
            return ".rodata.cst";
    }

    for(auto& p:prefix)
        if(Name==p||hasPrefix(Name,p))
            return p;

    return Name;
}

#include "Section.h"
#include "ObjectFile.h"
#include "Stringlib.h"

Section::Section(ObjectFile* ObjFile,void* ShdrAddr):SectionHeader(ShdrAddr){
    Content=ObjFile->getSectionAddr(this);
}

const static std::string prefix[]={
    ".text.", ".data.rel.ro.", ".data.", ".rodata.", ".bss.rel.ro.", ".bss.",
	".init_array.", ".fini_array.", ".tbss.", ".tdata.", ".gcc_except_table.",
	".ctors.", ".dtors.",
};

std::string Section::getOutputName(){
    auto Name=getName();
    auto Flag=loadComponentAs<uint64_t>(sh_flags);

    if ((Name == ".rodata" || hasPrefix(Name,".rodata.")) &&
		Flag&SHF_MERGE != 0) {
		if (Flag&SHF_STRINGS != 0) {
			return ".rodata.str";
		} else {
			return ".rodata.cst";
		}
	}

    for(auto& pre:prefix){
        auto stem=pre.substr(0,pre.size()-1);
        if(Name==stem||hasPrefix(Name,pre))
            return stem;
    }

	return Name;
}

/*
// SectionFragment: Content, Size, P2Align, isAlive
// create SectionFragment and insert it into MergedSection
// check out the symbols in this file and find the corresponding symbol, and associate it with the SectionFragment
*/

void Section::splitSection(MergeableSection& MergeableSection){
    auto Flags=getShFlags();
    auto [Content,Size]=getContent();
    
    auto Parent=MergeableSection.Parent;

    auto getp2align=[&](uint64_t Alignment){
        uint8_t p2align=0;
        for(auto i=0;i<64;i++){
            if(Alignment&(1<<i)){
                p2align=i;
                break;
            }
        }
        return p2align;
    };
    auto p2align=getp2align(getShAlign());
    MergeableSection.P2Align=p2align;

    if(Flags&SHF_STRINGS!=0){
        // merge string, which will have non-static size
        auto StrideAlign=getShEntSize();
        for(uint64_t Off=0;Off<Size;){
            auto Ptr=(char*)Content+Off;
            // 返回第一个为 0 的 offset
            auto Strlen=findNullOffset(Ptr,StrideAlign)+StrideAlign;
            MergeableSection.Fragments.emplace_back(Off,Parent->insertFragment(Ptr,Strlen,p2align,true));
            Off+=Strlen;
        }
    } else{
        auto EntSize=getShEntSize();
        assert(EntSize!=0&&Size%EntSize==0);

        uint64_t Off=0;
        for(auto ptr=Content,end=Content+Size;ptr<end;ptr+=EntSize){
            MergeableSection.Fragments.emplace_back(Off,Parent->insertFragment(ptr,EntSize,p2align,true));
            Off+=EntSize;
        }
    }
}
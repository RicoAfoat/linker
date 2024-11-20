#include <fstream>
#include <iostream>
#include <cassert>
#include <iomanip>

#include "ObjectFile.h"
#include "InputSection.h"
#include "Symbol.h"
#include "mergeablesection.h"
#include "mergedsection.h"
#include "Output.h"
#include "Stringlib.h"

void ObjectFile::initFileStructure(){
    InputFile::initFileStructure();
    this->SymtabSec=this->findSection(SHT_SYMTAB);
    if(this->SymtabSec!=nullptr){
        this->FirstGlobal=this->SymtabSec->sh_info;
        this->fillupElfSyms(this->SymtabSec);
        this->SymbolStrtab=this->getBytesFromIdx(this->SymtabSec->sh_link);
    }
    initSections();
    initSymbols();
    initMergeableSections();
}

void ObjectFile::initSections(){
    Sections=std::vector<std::unique_ptr<InputSection>>(ElfSections.size());
    for(int i=0,limi=ElfSections.size();i<ElfSections.size();i++){
        auto elfsec=ElfSections[i];
        switch (elfsec->sh_type)
        {
        case SHT_GROUP:
        case SHT_SYMTAB:
        case SHT_STRTAB:
        case SHT_REL:
        case SHT_RELA:
        case SHT_NULL:
            break;
        case SHT_SYMTAB_SHNDX:
            fillupSymtabShndxSec(elfsec);
            break;
        default:
            Sections[i]=std::make_unique<InputSection>(this,i);
            break;
        }
    }
}

void ObjectFile::fillupSymtabShndxSec(Shdr* s){
    auto [Content,Size]=this->getBytesFromIdx(s->sh_link);
    auto nums=Size/4;
    this->SymtabShndxSec=std::pair<uint32_t*,size_t>((uint32_t*)Content,nums);
}

void ObjectFile::initSymbols(){
    if(this->SymtabSec==nullptr)
        return;
    
    for(int i=0;i<FirstGlobal;i++)
        LocalSymbols.push_back(Symbol(""));
    LocalSymbols[0].File=this;

    for(int i=0;i<FirstGlobal;i++){
        auto esym=ElfSyms[i];
        auto& sym=LocalSymbols[i];
        sym.Name=ElfGetName(SymbolStrtab,esym->st_name);
        sym.File=this;
        sym.Value=esym->st_value;
        sym.SymIdx=i;

        if(!IsAbs(esym))
            sym.setInputSection(Sections[getShndx(esym,i)].get());
    }

    for(int i=0;i<FirstGlobal;i++)
        Symbols.push_back(&LocalSymbols[i]);

    for(int i=FirstGlobal;i<ElfSyms.size();i++){
        auto esym=ElfSyms[i];
        auto name=ElfGetName(SymbolStrtab,esym->st_name);
        Symbols.push_back(Symbol::getSymbolByName(name));
    }
}

uint64_t ObjectFile::getShndx(Sym* esym,int idx){
    assert(idx>=0&&idx<ElfSyms.size());
    if(esym->st_shndx==SHN_XINDEX){
        assert(idx<SymtabShndxSec.second);
        return SymtabShndxSec.first[idx];
    }
    return esym->st_shndx;
}

void ObjectFile::resolveSymbols(){
    for(auto i=FirstGlobal;i<ElfSyms.size();i++){
        auto esym=ElfSyms[i];
        auto sym=Symbols[i];
        if(IsUndef(esym))
            continue;
        
        InputSection* isec=nullptr;
        if(!IsAbs(esym)){
            isec=getSection(esym,i);
            if(isec==nullptr)
                continue;
        }

        if(sym->File!=nullptr)continue;
        sym->File=this;
        sym->setInputSection(isec);
        sym->Value=esym->st_value;
        sym->SymIdx=i;
    }
}

InputSection* ObjectFile::getSection(Sym* esym,int idx){
    return Sections[getShndx(esym,idx)].get();
}

void ObjectFile::markLiveObjects(std::function<void(ObjectFile*)> f){
    assert(isAlive);
    for(int i=FirstGlobal;i<ElfSyms.size();i++){
        auto esym=ElfSyms[i];
        auto sym=Symbols[i];
        
        if(sym->File==nullptr)
            continue;

        if(IsUndef(esym)&&!sym->File->isAlive){
            sym->File->isAlive=true;
            std::cerr<<"Marking file as alive: "<<sym->File->getFileName()<<std::endl;
            f(sym->File);
        }
    }
}

void ObjectFile::clearSymbols(){
    for(int i=FirstGlobal;i<ElfSyms.size();i++){
        auto sym=Symbols[i];
        if(sym->File==this)
            sym->clear();
    }
}

void ObjectFile::initMergeableSections(){
    MAbleSections=std::vector<std::unique_ptr<MergeableSection>>(Sections.size());

    for(int i=0,limi=Sections.size();i<limi;i++){
        auto isec=Sections[i].get();
        if(isec!=nullptr && isec->isAlive && isec->getShdr()->sh_flags&SHF_MERGE!=0){
            MAbleSections[i].reset(splitSection(isec));
            isec->isAlive=false;
        }
    }
}

static size_t findNull(uint8_t* ptr,size_t entsize,size_t Limit){
    for(auto i=0;i<=Limit-entsize;i+=entsize){
        if(AllZero((char*)ptr+i,entsize))
            return i;
    }
    assert(0&&"unreachable");
}

MergeableSection* ObjectFile::splitSection(InputSection* isec){
    auto [Content,Size]=isec->Content;
    auto shdr=isec->getShdr();

    auto MAS=new MergeableSection();
    MAS->Parent=MergedSection::getMergedSection(
        std::string(isec->getName()),
        shdr->sh_type,
        shdr->sh_flags
    );
    MAS->P2Align=isec->P2Align;

    if(shdr->sh_flags&SHF_STRINGS){
        for(auto offset=0;offset<Size;){
            auto end=findNull(Content+offset,shdr->sh_entsize,Size-offset);
            
            auto view=std::string_view((char*)Content+offset,end);
            MAS->Strs.push_back(view);
            MAS->FragOffsets.push_back(offset);

            offset+=end;
        }
    }
    else{
        assert(Size%shdr->sh_entsize==0);
        for(auto offset=0;offset<Size;offset+=shdr->sh_entsize){
            auto view=std::string_view((char*)Content+offset,shdr->sh_entsize);
            MAS->Strs.push_back(view);
            MAS->FragOffsets.push_back(offset);
        }
    }

    return MAS;
}

void ObjectFile::registerSectionPieces(){
    for(auto& masec:MAbleSections){
        if(masec==nullptr)
            continue;
        
        masec->Fragments=std::vector<SectionFragment*>(masec->Strs.size(),nullptr);
        for(auto& Str:masec->Strs){
            auto frag=masec->Parent->Insert(Str,masec->P2Align);
            masec->Fragments.push_back(frag);
        }
    }

    for(decltype(ElfSyms.size()) i=1,limi=ElfSyms.size();i<limi;i++){
        auto sym=Symbols[i];
        auto esym=ElfSyms[i];
        if(IsAbs(esym)||IsUndef(esym)||IsCommon(esym))
            continue;
        
        auto masec=MAbleSections[getShndx(esym,i)].get();
        if(masec==nullptr)
            continue;

        auto [frag,offset]=masec->getFragment(esym->st_value);
        assert(frag!=nullptr);
        sym->setSectionFragment(frag);
        sym->Value=offset;
    }
}

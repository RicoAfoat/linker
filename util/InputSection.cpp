#include "InputSection.h"
#include "ObjectFile.h"
#include "OutputSection.h"
#include "Symbol.h"
#include "Context.h"
#include "Singleton.h"
#include <cstring>
#include <cassert>

template<typename T>
void datacopy(void* dst,T src){
    memcpy(dst,&src,sizeof(T));
}

template<typename T>
T dataread(void* src){
    return *(T*)src;
}

template<typename T>
T Bits(T val, int hi, int lo) {
    return (val >> lo) & ((T(1) << (hi - lo + 1)) - 1);
}

template<typename T>
T Bit(T val, int bit) {
    return Bits(val, bit, bit);
}

// 定义函数
uint32_t itype(uint32_t val) {
    return val << 20;
}

uint32_t stype(uint32_t val) {
    return (Bits(val,11,5)<<25)|(Bits(val,4,0)<<7);
}

uint32_t btype(uint32_t val) {
    return (Bit(val,12)<<31)|(Bits(val,10,5)<<25)|(Bits(val,4,1)<<8)|(Bit(val,11)<<7);
}

uint32_t utype(uint32_t val) {
    return (val+0x800)&(0xfffff000);
}

uint32_t jtype(uint32_t val) {
    return (Bit(val,20)<<31)|(Bits(val,10,1)<<21)|(Bit(val,11)<<20)|(Bits(val,19,12)<<12);
}

uint16_t cbtype(uint16_t val) {
    return 
        (Bit(val,8)<<12)|
        (Bit(val,4)<<11)|
        (Bit(val,3)<<10)|
        (Bit(val,7)<<6)|
        (Bit(val,6)<<5)|
        (Bit(val,2)<<4)|
        (Bit(val,1)<<3)|
        (Bit(val,5)<<2);
}

uint16_t cjtype(uint16_t val){
    return
        (Bit(val, 11)<<12) |
        (Bit(val, 4)<<11) |
        (Bit(val, 9)<<10) |
        (Bit(val, 8)<<9) |
        (Bit(val, 10)<<8) |
        (Bit(val, 6)<<7) |
        (Bit(val, 7)<<6) |
        (Bit(val, 3)<<5) |
        (Bit(val, 2)<<4) |
        (Bit(val, 1)<<3) |
        (Bit(val, 5)<<2) ;
}

void writeItype(void* dst,uint32_t val){
    auto mask=uint32_t(0b0000000000011111111111111111111);
    datacopy<uint32_t>(dst,(dataread<uint32_t>(dst)&mask)|itype(val));
}

void writeStype(void* dst, uint32_t val) {
    auto mask = uint32_t
    (0b0000001111111111111000001111111);
    datacopy<uint32_t>(dst, (dataread<uint32_t>(dst) & mask) | stype(val));
}

void writeBtype(void* dst, uint32_t val) {
    auto mask = uint32_t
    (0b0000001111111111111000001111111);
    datacopy<uint32_t>(dst, (dataread<uint32_t>(dst) & mask) | btype(val));
}

void writeUtype(void* dst, uint32_t val) {
    auto mask = uint32_t    (0b0000000000000000000111111111111);
    datacopy<uint32_t>(dst, (dataread<uint32_t>(dst) & mask) | utype(val));
}

void writeJtype(void* dst, uint32_t val) {
    auto mask = uint32_t(0b0000000000000000000111111111111);
    datacopy<uint32_t>(dst, (dataread<uint32_t>(dst) & mask) | jtype(val));
}

void setRs1(void* dst,uint32_t rs1){
    auto mask=uint32_t(0b1111111111100000111111111111111);
    datacopy<uint32_t>(dst,dataread<uint32_t>(dst)&mask);
    datacopy<uint32_t>(dst,dataread<uint32_t>(dst)|(rs1<<15));
}

uint64_t signExtend(uint64_t val,int size){
    return uint64_t(int64_t(val<<(63-size))>>(63-size));
}

InputSection::InputSection(ObjectFile* f,uint32_t shndx){
    this->File=f;
    this->Shndx=shndx;
    this->Content=f->getBytesFromIdx(shndx);
    this->isAlive=true;

    auto shdr=getShdr();
    assert((shdr->sh_flags&SHF_COMPRESSED)==0);

    auto toP2Align=[](uint64_t alignment) ->uint8_t {
        for(uint8_t i=0;i<64;i++)
            if(alignment&((uint64_t)1<<i))
                return i;
        return 0;
    };

    this->P2Align=toP2Align(shdr->sh_addralign);
    // this->OutSec=OutputSection::getOutputSection(std::string(this->getName()),shdr->sh_type,shdr->sh_flags);
    // this->OutSec->Member.push_back(this);
}

Shdr* InputSection::getShdr(){
    assert(Shndx<File->ElfSections.size());
    return File->ElfSections[Shndx];    
}

std::string_view InputSection::getName(){
    auto shdr=getShdr();
    return ElfGetName(File->Shstrtab,shdr->sh_name);
}

uint64_t InputSection::getAddr(){
    return this->OutSec->getShdr()->sh_addr+this->offset;
}

std::pair<Rela*,size_t> InputSection::getRels(){
    if(RelsecIdx==UINT32_MAX)
        return std::pair<Rela*,size_t>(nullptr,0);
    auto [Content,Size]=File->getBytesFromIdx(RelsecIdx);
    return std::pair<Rela*,size_t>((Rela*)Content,Size/sizeof(Rela));
}

void InputSection::ScanRelocations(){
    auto [rela,size]=getRels();
    for(decltype(size) i=0;i<size;i++){
        auto sym=File->Symbols[ELF64_R_SYM(rela[i].r_info)];
        if(sym->File==nullptr)continue;
        if(ELF64_R_TYPE(rela[i].r_info)==R_RISCV_TLS_GOT_HI20){
            sym->Flags|=CustomSymbolFlags::NeedsGotTp;
        }
    }
}

void InputSection::WriteTo(uint8_t* dst){
    if(getShdr()->sh_type==SHT_NOBITS||getShdr()->sh_size==0)
        return;
    
    dst+=this->offset;
    auto [Content,Size]=this->Content;
    memcpy(dst,Content,Size);

    if((getShdr()->sh_flags&SHF_ALLOC)!=0){
        ApplyRelocations(std::make_pair(dst,Size));
    }
}

void InputSection::ApplyRelocations(std::pair<uint8_t*,size_t> SectionContent){
    auto [rela,relaSize]=getRels();
    // 这个在buffer中
    auto [base,secSize]=SectionContent;

    for(decltype(relaSize) i=0;i<relaSize;i++){
        auto relentry=rela[i];
        if(ELF64_R_TYPE(relentry.r_info)==R_RISCV_NONE || ELF64_R_TYPE(relentry.r_info)==R_RISCV_RELAX){
            continue;
        }
        auto sym=File->Symbols[ELF64_R_SYM(relentry.r_info)];
        // 我们需要写入的位置, buffer的初始RAM地址+对应的OutputSection的offset+InputSection的offset+r_offset
        auto loc=base+relentry.r_offset;
        if(sym->File==nullptr)continue;
        
        // Symbol的虚拟地址
        auto S=sym->getAddr();
        auto A=relentry.r_addend;
        // 需要改写的位置的虚拟地址
        auto P=this->getAddr()+relentry.r_offset;

        switch (ELF64_R_TYPE(relentry.r_info))
        {
        case R_RISCV_32:
            datacopy<uint32_t>(loc,S+A);
            break;
        case R_RISCV_64:
            datacopy<uint64_t>(loc,S+A);
            break;
        case R_RISCV_BRANCH:
            writeBtype(loc,S+A-P);
            break;
        case R_RISCV_JAL:
            writeJtype(loc,S+A-P);
            break;
        case R_RISCV_CALL:
        case R_RISCV_CALL_PLT:
            writeUtype(loc,S+A-P);
            writeItype(loc+4,S+A-P);
            break;
        case R_RISCV_TLS_GOT_HI20:
            datacopy<uint32_t>(loc,uint32_t(sym->getGotTpAddr()+A-P));
            break;
        case R_RISCV_PCREL_HI20:
            datacopy<uint32_t>(loc,uint32_t(S+A-P));
            break;
        case R_RISCV_HI20:
            datacopy<uint32_t>(loc,uint32_t(S+A));
            break;
        case R_RISCV_LO12_I:
        case R_RISCV_LO12_S:
            {
                auto val=S+A-Singleton<Context>().TpAddr;
                if(ELF64_R_TYPE(relentry.r_info)==R_RISCV_LO12_I){
                    writeItype(loc,uint32_t(val));
                } else{
                    writeStype(loc,uint32_t(val));
                }
                if(signExtend(val,11)==val){
                    setRs1(loc,4);
                }
                break;
            }
        default:
            break;
        }
    }

    for(decltype(relaSize) i=0;i<relaSize;i++){
        auto relentry=rela[i];
        switch (ELF64_R_TYPE(relentry.r_info)){
            case R_RISCV_PCREL_LO12_I:
            case R_RISCV_PCREL_LO12_S:
            {
                auto sym=File->Symbols[ELF64_R_SYM(relentry.r_info)];
                assert(sym->InputSec==this);
                auto loc=base+relentry.r_offset;
                auto val=dataread<uint32_t>(base+sym->Value);
                if(ELF64_R_TYPE(relentry.r_info)==R_RISCV_PCREL_LO12_I){
                    writeItype(loc,uint32_t(val));
                } else{
                    writeStype(loc,uint32_t(val));
                }
                break;
            }
            default:
                break;
        }
    }

    for(decltype(relaSize) i=0;i<relaSize;i++){
        auto relentry=rela[i];
        switch (ELF64_R_TYPE(relentry.r_info)){
            case R_RISCV_PCREL_HI20:
            case R_RISCV_TLS_GOT_HI20:
            {
                auto loc=base+relentry.r_offset;
                auto val=dataread<uint32_t>(loc);
                datacopy<uint32_t>(loc,dataread<uint32_t>(this->Content.first+relentry.r_offset));
                writeUtype(loc,val);
                break;
            }
            default:
                break;
        }
    }
}

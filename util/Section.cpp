#include "Section.h"
#include "ObjectFile.h"

Section::Section(ObjectFile* ObjFile,void* ShdrAddr):SectionHeader(ShdrAddr){
    Content=ObjFile->getSectionAddr(this);
}
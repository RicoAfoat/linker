#pragma once
#include "InputFile.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <functional>
#include <queue>

class InputSection;
class MergeableSection;

struct ObjectFile:public InputFile {
    Shdr* SymtabSec;
    std::pair<uint32_t*,size_t> SymtabShndxSec;
    std::vector<std::unique_ptr<InputSection>> Sections;

    std::vector<std::unique_ptr<MergeableSection>> MAbleSections;

public:    
    void initFileStructure() override;
    void initSections();
    void initSymbols();
    void initMergeableSections();

    void resolveSymbols();

    void fillupSymtabShndxSec(Shdr* s);

    uint64_t getShndx(Sym*,int);

    InputSection* getSection(Sym* esym,int idx);

    void markLiveObjects(std::function<void(ObjectFile*)> f);

    void clearSymbols();

    MergeableSection* splitSection(InputSection* isec);

    void registerSectionPieces();

    void scanRelocations();
};
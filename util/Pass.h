#pragma once
#include <cstdint>
namespace Passes{

void resolveSymbols();
void markLiveObjects();
void registerSectionPieces();
void createSyntheticSections();
uint64_t getOutputFileSize();
void writeOutputFile();
} // namespace Passes

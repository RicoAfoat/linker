#pragma once
#include <cstdint>
class Chunk;
namespace Passes{

void resolveSymbols();
void markLiveObjects();
void registerSectionPieces();
void createSyntheticSections();
void writeOutputFile();
void computeSectionSizes();
void collectOutputSections();
void BinSections();
uint64_t setOutputSectionOffsets();
void sortOutputSections();
void computeMergedSectionSizes();
} // namespace Passes

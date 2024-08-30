#pragma once
#include <cstdint>
#include <windows.h>
#include <psapi.h>
#include <string>
namespace utils::memory {
void *search(const std::string mod, const char *pPattern, uint8_t wildcard);
void *search(uint8_t *pScanPos, size_t scanSize, const uint8_t *pPattern, size_t patternSize, uint8_t wildcard);

MODULEINFO GetModuleInfo(const std::string mod);

}// namespace utils::memory
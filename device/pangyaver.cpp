#include "pangyaver.h"

#include <windows.h>
#include <psapi.h>
#include <algorithm>
#include <functional>

/*
 * PangYa version detection algorithm.
 *
 * In order to detect what version of PangYa we're loaded into, we use a simple algorithm:
 * - Find the contiguous regions of memory that reside at the binary's base load address.
 * - Use a Boyer-Moore search in that region to find the format string used to output stack.log.
 * - Use a Boyer-Moore search in that region to find the push call that pushes that format string on the stack.
 * - Since stack arguments are pushed right-to-left, search up 20 bytes until we find the version string push.
 *   (It is the only other push from an absolute address in the argument list, so this is easy.)
 *
 * NOTE: This only determines the version. Approaches for detecting game region are still needed.
 * Maybe we can keep a table of PDB path <> Region?
 */

constexpr std::string_view g_stackFormatString = "%d, %s, %d, %s, %s\n";

static LPVOID GetPangYaAddress() {
	MODULEINFO moduleInfo;
	HMODULE hExeModule = GetModuleHandle(nullptr);

	GetModuleInformation(GetCurrentProcess(), hExeModule, &moduleInfo, sizeof moduleInfo);
	return moduleInfo.lpBaseOfDll;
}

std::string_view ScanPangYaVersion() {
	MEMORY_BASIC_INFORMATION memoryRegion;
	const auto* memBegin = LPCSTR(GetPangYaAddress());
	const auto* memEnd = memBegin;

	// Find length of contiguous region.
	for (; VirtualQuery(memEnd, &memoryRegion, sizeof memoryRegion) && memoryRegion.State != MEM_FREE; memEnd += memoryRegion.RegionSize) {}

	// Find stack print pattern.
	const auto *it = std::search(memBegin, memEnd,
	                      std::boyer_moore_searcher(
							  g_stackFormatString.begin(), g_stackFormatString.end()));
	if (it == memEnd) {
		return "";
	}

	// Find push instruction for stack print.
	std::string pushStackFormatString = {0x68, 0x00, 0x00, 0x00, 0x00};
	memcpy(&pushStackFormatString[1], &it, sizeof&it);
	it = std::search(memBegin, memEnd,
		std::boyer_moore_searcher(
			pushStackFormatString.begin(), pushStackFormatString.end()));
	if (it == memEnd) {
		return "";
	}

	// Work backwards and try to find version string offset.
	for (int i = 1; i < 20; i++) {
		if (it[-i] == 0x68) {
			LPCSTR address = *reinterpret_cast<const LPCSTR*>(it - i + 1);
			if (address >= memBegin && address < memEnd) {
				return std::string_view(address);
			}
		}
	}

	return "";
}

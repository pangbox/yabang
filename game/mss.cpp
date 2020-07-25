#include "mss.h"

#include <cstdlib>

#ifdef _WIN64
constexpr TCHAR g_mssLib[] = TEXT("mss64.dll");
#elif _WIN32
constexpr TCHAR g_mssLib[] = TEXT("mss32.dll");
#endif

AIL::AIL() {
	#ifdef _WIN32
	this->mss = LoadLibrary(g_mssLib);
	if (!this->mss) {
		return;
	}
#define ail_func(name, return_type, arg_list, param_list) \
	ptr_##name = reinterpret_cast<PFN_AIL_##name>(GetProcAddress(this->mss, "AIL_" #name));
#include "mssfunc.x"
#undef ail_func
	#endif
}

AIL::~AIL() {
	if (!this->mss) {
		return;
	}
	FreeLibrary(this->mss);
}

bool AIL::HaveMss() const {
	return this->mss != nullptr;
}

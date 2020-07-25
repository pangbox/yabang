#pragma once

/*
 * We do not have access to the Miles Sound System SDK, but we want to
 * be able to use MSS (at least temporarily.) This shim contains enough code
 * to load MSS and use functionality needed by PangYa. We should aim to
 * replace all of this with open source alternatives.
 *
 * Most importantly, Yabang must NEVER be redistributed with copies of MSS.
 */

#include <cstdint>
#include <exception>
#include <windows.h>

// Opaque pointer types.
typedef void* HSTREAM;
typedef void* HSAMPLE;
typedef void* HDIGDRIVER;

// Enumerations
typedef enum
{
	MSS_MC_INVALID = 0,
	MSS_MC_MONO = 1,   
	MSS_MC_STEREO = 2,
	MSS_MC_USE_SYSTEM_CONFIG = 0x10,
	MSS_MC_HEADPHONES = 0x20,
	MSS_MC_DOLBY_SURROUND = 0x30,
	MSS_MC_SRS_CIRCLE_SURROUND = 0x40,
	MSS_MC_40_DTS = 0x48,
	MSS_MC_40_DISCRETE = 0x50,
	MSS_MC_51_DTS = 0x58,
	MSS_MC_51_DISCRETE = 0x60,
	MSS_MC_61_DISCRETE = 0x70,
	MSS_MC_71_DISCRETE = 0x80,
	MSS_MC_81_DISCRETE = 0x90,
	MSS_MC_DIRECTSOUND3D = 0xA0,
	MSS_MC_EAX2 = 0xC0,
	MSS_MC_EAX3 = 0xD0,
	MSS_MC_EAX4 = 0xE0
} MSS_MC_SPEC;

// Function pointer types.
#define ail_func(name, return_type, arg_list, param_list) \
	typedef return_type (__stdcall* PFN_AIL_##name) arg_list;
#include "mssfunc.x"
#undef ail_func

class MssNotFound final : public std::exception {
public:
	MssNotFound() : std::exception("mss module not found") {}
};

// Small gadget that lets us return the zero value for a given type, or
// nothing for void.
namespace {
	template<typename T>
	T ReturnEmpty() { return{}; }
	template<>
	void ReturnEmpty() { }
}

struct AIL {
	AIL();
	~AIL();

	AIL(const AIL&) = delete;
	AIL(AIL&&) = delete;
	AIL& operator=(const AIL&) = delete;
	AIL& operator=(AIL&&) = delete;

	bool HaveMss() const;
	
private:
	HMODULE mss;
#define ail_func(name, return_type, arg_list, param_list) \
private: \
	PFN_AIL_##name ptr_##name; \
public: \
	return_type name arg_list const { \
		if (ptr_##name != nullptr) { return ptr_##name param_list; } \
		return ReturnEmpty<return_type>(); \
	}
#include "mssfunc.x"
#undef ail_func
};

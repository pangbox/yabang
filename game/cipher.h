#pragma once
#include <cstdint>

constexpr static uint32_t g_xteaKeyKr[] = { 0x485B576, 0x5148E02, 0x5141D96, 0x28FA9D6 };

void Decipher(const uint32_t* v, uint32_t* w, const uint32_t* k);

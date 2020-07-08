#pragma once
#include <cstdint>

struct sPakinfo {
	uint8_t type;
	uint16_t next;
	uint32_t offset;
	uint32_t packedsize;
	uint32_t size;
	char filename[1];
};

#pragma once
#include <cstdint>

class cFile;
struct sPakinfo;

struct sPak {
	int num;
	cFile* fp;
	int offset;
	uint16_t hash[256];
	void* lock;
	sPakinfo** list;
	sPak* next;
};

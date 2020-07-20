#include "cipher.h"

void Decipher(const uint32_t* v, uint32_t* w, const uint32_t* k) {
	uint32_t sum = 0xE3779B90;
	w[0] = v[0];
	w[1] = v[1];
	for (int i = 0; i < 16; i++) {
		w[1] -= (w[0] << 4 ^ w[0] >> 5) + w[0] ^ sum + k[sum >> 11 & 3];
		sum += 0x61C88647;
		w[0] -= (w[1] << 4 ^ w[1] >> 5) + w[1] ^ sum + k[sum & 3];
	}
}

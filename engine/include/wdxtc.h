#pragma once
#include <cstdint>
#include <ddraw.h>

struct Color32 {
	union {
		uint32_t clr;
		struct {
			uint8_t b, g, r, a;
		};
	};
};

struct DxtcColorBlock {
	uint16_t color0;
	uint16_t color1;
	uint8_t row[4];
};

struct DxtcExpAlphaBlock {
	uint16_t row[4];
};

struct DxtcLinearAlphaBlock3Bit {
	uint8_t a0;
	uint8_t a1;
	uint8_t stuff[6];
};

class WDXTC {
public:
	WDXTC();
	WDXTC(const WDXTC&) = delete;
	WDXTC(WDXTC&&) = delete;
	~WDXTC();

	WDXTC& operator=(const WDXTC&) = delete;
	WDXTC& operator=(WDXTC&&) = delete;

	LPDDSURFACEDESC2 GetDdsd2();
	[[nodiscard]] uint8_t* GetDecompData() const;
	bool Load(uint8_t* pData, int iSize);

private:
	void DecodeColor(Color32 *pDecomp, const DxtcColorBlock *pBlock, Color32 *aClr32) const;
	void DecodeExpAlphaBlock(Color32 *pDecomp, const DxtcExpAlphaBlock *pAlpha) const;
	void DecodeLinearAlphaBlock3Bit(Color32 *pDecomp, const DxtcLinearAlphaBlock3Bit *pAlpha) const;
	void DecompressBlockDxt1(uint8_t* pDecomp, const DxtcColorBlock *pBlock) const;
	void DecompressBlockDxt3(uint8_t* pDecomp, const DxtcColorBlock *pBlock) const;
	void DecompressBlockDxt5(uint8_t* pDecomp, const DxtcColorBlock *pBlock) const;
	bool DecompressDxtc();
	void DecompressDxt1() const;
	void DecompressDxt3() const;
	void DecompressDxt5() const;
	static void GetColorFromBlock(Color32 *aClr32, const DxtcColorBlock *pBlock);
	void SaveAsBmp(char *filename) const;

	DDSURFACEDESC2 m_ddsd2;
	uint8_t* m_pCompData;
	uint8_t* m_pDecompData;
};

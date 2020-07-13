#pragma once
#include "wfont.h"

typedef struct tagWTITLEFONT {
	char** filename;
	int numPages;
	int fontw;
	int fonth;
	int texw;
	int texh;
	int flag;
	char* pCharSet;
} WTITLEFONT;

class WTitleFont : public WFont {
public:
	WTitleFont();
	
private:
	WTITLEFONT m_info{};
	int* m_iTexIndex{};
	float* m_fWidthIndex{};
	char* m_pCharSet{};
	int m_numCharSet{};
	int m_numPerPage{};
	int m_numWidth{};
	int m_numTex{};
};

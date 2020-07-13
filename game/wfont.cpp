#include "wfont.h"

WFont::WFont() = default;

void WFont::SetMode(eFontStyle type) {
	this->m_eType = type;
}

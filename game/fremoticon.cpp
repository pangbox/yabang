#include "fremoticon.h"

void FrEmoticon::SetAnim(bool enable) {
	this->m_EnableAnim = enable;
}

void FrEmoticon::SetAnimTime(unsigned int time) {
	this->m_AnimStart = time;
}

int FrEmoticon::GetWidth() const {
	return this->m_selWidth;
}

int FrEmoticon::GetHeight() const {
	return this->m_selHeight;
}

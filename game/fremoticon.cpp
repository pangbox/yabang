#include "fremoticon.h"

void FrEmoticon::SetAnim(bool enable) {
	this->m_EnableAnim = enable;
}

void FrEmoticon::SetAnimTime(unsigned int time) {
	this->m_AnimStart = time;
}
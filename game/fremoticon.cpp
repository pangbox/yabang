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

int FrEmoticon::GetIconNum() const {
	return this->m_Emoticons.size();
}

int FrEmoticon::GetIconIndex(const char* key) {
	if (!key || !key[0]) {
		return -1;
	}
	for (size_t i = 0; i < this->m_Emoticons.size(); i++) {
		sEmoticon* emoticon = this->m_Emoticons[i];
		if (!emoticon) {
			break;
		}
		if (_strcmpi(emoticon->Name.c_str(), key)) {
			if (key[0] == '-' && !_strcmpi(emoticon->Name.c_str(), key + 1)) {
				return i;
			}
			for (const auto& alias : emoticon->Alias) {
				if (!_strcmpi(alias.c_str(), key)) {
					return i;
				}
				if (key[0] == '-' && !_strcmpi(alias.c_str(), key + 1)) {
					return i;
				}
			}
		}
	}
	return -1;
}

#include "fremoticon.h"

#include <windows.h>
#include <timeapi.h>
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "wmath.h"
#include "woverlay.h"
#include "wrecvview.h"

FrEmoticon::FrEmoticon(FrWndManager* pManager)
	: m_pManager(pManager) {}

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

int FrEmoticon::GetIconIndex(const char* key) const {
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

const char* FrEmoticon::GetIconName(int index) const {
	return this->m_Emoticons[index]->Name.c_str();
}

void FrEmoticon::GetAlias(std::string (&buffer)[2], int index) const {
	if (!this->m_Emoticons[index]->Alias[0].empty()) {
		buffer[0] = this->m_Emoticons[index]->Alias[0];
	}

	if (!this->m_Emoticons[index]->Alias[1].empty()) {
		buffer[1] = this->m_Emoticons[index]->Alias[1];
	}
}

bool FrEmoticon::Draw(int icon, const WRect& dst, unsigned int diffuse, bool flip) const {
	if (icon < 0) {
		return false;
	}
	FrGraphicInterface* pGdi = this->m_pManager->GetGDI();
	if (!pGdi) {
		return false;
	}
	unsigned int frame = (timeGetTime() - this->m_AnimStart) / (1000 / this->m_Fps);
	sEmoticon* emoticon = this->m_Emoticons[icon];
	WOverlay* overlay = this->m_overlay;
	if (this->m_EnableAnim && !emoticon->Frames.empty()) {
		icon = emoticon->Frames[frame % emoticon->Frames.size()];
		overlay = emoticon->Overlay;
	}
	int selX = icon % this->m_selXNum;
	int selY = icon % this->m_selNum / this->m_selXNum;
	auto height = static_cast<float>(overlay->GetHeight());
	auto width = static_cast<float>(overlay->GetWidth());
	diffuse = (static_cast<int>(pGdi->GetAlpha() * 255.0) << 24) | 0xFFFFFF;
	if (flip) {
		WRect srcRect;
		srcRect.x = static_cast<float>(this->m_selWidth * (selX + 1)) / width;
		srcRect.y = static_cast<float>(selY * this->m_selHeight) / height;
		srcRect.w = static_cast<float>(-this->m_selWidth) / width;
		srcRect.h = static_cast<float>(this->m_selHeight) / height;
		overlay->Render(g_view, srcRect, dst, 0, diffuse, 0.0, 0);
	} else {
		WRect srcRect;
		srcRect.x = static_cast<float>(selX * this->m_selWidth) / width;
		srcRect.y = static_cast<float>(selY * this->m_selHeight) / height;
		srcRect.w = static_cast<float>(this->m_selWidth) / width;
		srcRect.h = static_cast<float>(this->m_selHeight) / height;
		overlay->Render(g_view, srcRect, dst, 0, diffuse, 0.0, 0);
	}
	return true;
}

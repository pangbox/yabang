#pragma once
#include <map>
#include <string>
#include <vector>

#include "wmath.h"

class WOverlay;
class FrWndManager;

class FrEmoticon {
public:
	struct sEmoticon {
		sEmoticon() = default;

		std::string Name{};
		std::string Alias[2]{};
		WOverlay* Overlay = nullptr;
		std::vector<int> Frames{};
	};

	FrEmoticon(FrWndManager* pManager);

	void SetAnim(bool enable);
	void SetAnimTime(unsigned int time);
	int GetWidth() const;
	int GetHeight() const;
	int GetIconNum() const;
	int GetIconIndex(const char* key) const;
	const char* GetIconName(int index) const;
	void GetAlias(std::string (&buffer)[2], int index) const;
	bool Draw(int icon, const WRect& dst, unsigned diffuse, bool flip) const;
	bool Draw(int icon, float x, float y, unsigned diffuse, bool flip) const;

protected:
	int m_Fps = 0;
	std::vector<sEmoticon*> m_Emoticons{};
	std::map<std::string, WOverlay*> m_ResMap{};
	bool m_EnableAnim = false;
	int m_AnimStart = 0;
	WOverlay* m_overlay = nullptr;
	int m_selWidth = 26;
	int m_selHeight = 24;
	int m_selXNum = 9;
	int m_selYNum = 10;
	int m_selNum = 90;
	FrWndManager* m_pManager;
};

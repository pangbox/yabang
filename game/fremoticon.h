#pragma once
#include <map>
#include <string>
#include <vector>

class WOverlay;
class FrWndManager;

class FrEmoticon {
public:
	struct sEmoticon {
		std::string Name;
		std::string Alias[2];
		WOverlay* Overlay;
		std::vector<int> Frames;
	};

	void SetAnim(bool enable);
	void SetAnimTime(unsigned int time);
	int GetWidth() const;
	int GetHeight() const;
	int GetIconNum() const;
	int GetIconIndex(const char* key) const;
	const char* GetIconName(int index) const;
	void GetAlias(std::string(&buffer)[2], int index) const;

protected:
	int m_Fps;
	std::vector<sEmoticon*, std::allocator<sEmoticon*>> m_Emoticons;
	std::map<std::string, WOverlay*> m_ResMap;
	bool m_EnableAnim;
	int m_AnimStart;
	WOverlay* m_overlay;
	int m_selWidth;
	int m_selHeight;
	int m_selXNum;
	int m_selYNum;
	int m_selNum;
	FrWndManager* m_pManager;
};

#pragma once
#include <list>
#include <string>

#include "frcmdtarget.h"
#include "wflags.h"
#include "wmath.h"

class FrWndManager;
class FrScrollBar;

class FrWnd {
private:
	struct sToolTipData {
		int bFixWnd;
		WPoint posFixWnd;
		unsigned int style;
	};

	WFlags m_nFlags;
	FrWnd* m_pParentWnd;
	FrCmdTarget* m_pOwner;
	std::list<FrWnd*> m_childList;
	WRect m_rect;
	WFlags m_dwStyle;
	std::string m_wndText;
	std::string m_wndName;
	float m_wndAlpha;
	float m_wndAlpha2;
	unsigned int m_nRefID;
	FrWndManager* m_pWndManager;
	FrScrollBar* m_pScrBar;
	float m_fadeTime;
	WRect m_iconRect;
	const char* m_szPushSound;
	float m_dblClickTimeout;
	bool m_dblClicked;
	WPoint m_dblClickPos;
	bool m_hoverOn;
	float m_hoverTime;
	float m_accHoverTime;
	std::basic_string<char, std::char_traits<char>, std::allocator<char> > m_wndToolTip;
	sToolTipData* m_pToolTipData;
};

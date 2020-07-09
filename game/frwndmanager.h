#pragma once
#include <list>
#include <string>

#include "frinputstate.h"
#include "wmath.h"

class FrElementDoc;
class FrDesktop;
class FrCursor;
class FrWnd;
class FrGraphicInterface;
class FrEmoticon;
class FrEdit;

class FrWndManager {
	friend class FrWnd;
public:

private:
	enum eFadeState {
		OPENING = 0x0,
		OPENED = 0x1,
	};

	FrElementDoc* m_pDoc;
	FrDesktop* m_pDesktop;
	FrCursor* m_pCursor;
	FrInputState m_istate;
	bool m_exclusiveKey;
	std::list<FrWnd*> m_topmostList;
	std::string m_layoutID;
	FrWnd* m_pCaptured;
	FrGraphicInterface* m_pDevice;
	unsigned int m_nRefIndex;
	int m_cursorIndex;
	FrEmoticon* m_pEmoticon;
	float m_fadeoutAlpha;
	eFadeState m_state;
	FrEdit* m_pFocusedEdit;
	WPoint m_caretPos;
	bool m_hidePrivacy;
};

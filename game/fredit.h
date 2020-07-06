#pragma once
#include <list>
#include <string>

#include "wflags.h"

class FrGuiItem;
class FrLine;

class FrEdit {
public:

private:
	enum eEditStyle {
		ES_EMOTICON = 0x1,
		ES_READ_ONLY = 0x2,
		ES_NO_EMOTICON = 0x4,
	};

	enum eEditTextType {
		ETT_FRONT = 0x0,
		ETT_COMP = 0x1,
		ETT_END = 0x2,
		MAX_ETT = 0x3,
	};

	enum eAlign {
		leftAlign = 0x0,
		centerAlign = 0x1,
		rightAlign = 0x2,
	};

	FrGuiItem* m_pItem;
	int m_lines;
	int m_lineHeight;
	int m_leftMargin;
	int m_topMargin;
	unsigned int m_font;
	unsigned int m_fontColor;
	unsigned int m_fontColor2;
	unsigned int m_bgColor;
	unsigned int m_borderColor;
	bool m_multiLine;
	int m_charLimit;
	float m_widthLimit;
	bool m_password;
	bool m_bCaretMove;
	WFlags m_editProperty;
	bool m_focusOff;
	bool m_bSymmetry;
	unsigned int m_dwCaretColor;
	unsigned int m_dwCaretColor2;
	bool m_bScrollUpdate;
	int m_OldCaretPos;
	eAlign m_align;
	bool m_vCenterAlign;
	std::string m_editText[3];
	std::string m_oldEditText;
	int m_selectedLine;
	std::list<FrLine*> m_lineList;
	float m_caret;
	char m_caretDelta;
	bool m_AutoLine;
};

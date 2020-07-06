#pragma once
#include <string>

#include "wmath.h"

class WFont;
class WOverlay;

class CChatMsg {
public:
	bool m_bPrinted;
	bool m_bCaretMove;
	int m_nMaxBytes;
	float m_fMaxWidth;
	WFont* m_pMaskedFont;
	WOverlay* m_pChatWindow[3];
	WRect m_rcChatWinSrc[3];
	WRect m_rcChatWinDst[3];
	float m_fVel;
	bool m_bActive;
	bool m_bInsert;
	bool m_bCompInProgress;
	bool m_bShowCaret;
	float m_fCaretTime;
	int m_iState;
	int m_iCompCode;
	int m_iPrevCode;
	int m_iConsolCode;
	char m_szChat[1024];
	char m_szComp[1024];
	char m_szExt[1024];
	bool m_abWide[1024];
	int m_iNumBytes;
	int m_iCurByte;
	int m_iNumChars;
	int m_iCurChar;
	int m_iSelByteS;
	int m_iSelByteE;
	int m_iNumSelBytes;
	int m_iSelCharS;
	int m_iSelCharE;
	int m_iNumSelChars;
	char m_szCopied[1024];
	char m_szCopiedExt[1024];
	bool m_abCopied[1024];
	int m_iNumCopyBytes;
	int m_iNumCopyChars;
	unsigned int m_lastMsgTime;
	char m_bBackupUiAlpha;
	bool m_bOpen;
	bool m_bToggle;
	std::string m_chatFacialUser;
	char m_bCompAttr[1024];
	unsigned int m_dwCompCls[256];
	unsigned int m_dwCompStrLen;
	unsigned int m_dwCompAttrLen;
	unsigned int m_dwCompClsLen;
};

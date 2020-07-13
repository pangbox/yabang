#pragma once
#include <list>
#include <string>

#include "frcmdtarget.h"
#include "frwndmanager.h"
#include "iobject.h"
#include "wflags.h"
#include "wmath.h"

class CChatMsg;
struct FrInputState;
class FrWndManager;
class FrScrollBar;
class FrToolTip;

enum eFrFlags {
	FWF_NONE = 0x0,
	FWF_TOOLTIPS = 0x1,
	FWF_VIEWFOCUS = 0x8,
	FWF_KEYFOCUS = 0x10,
	FWF_DESTROY = 0x20,
	FWF_FADEOUT = 0x40,
	FWF_INITED = 0x80,
	FWF_FADING = 0x100,
	FWF_TOPFOCUS = 0x200,
	FWF_FADING_EX = 0x400,
	FWF_FADEOUT_EX = 0x800,
	FWF_PRIVACY = 0x1000,
	FWF_FADEELEMENT = 0x2000,
};

enum eFrStyle : __int32 {
	FWS_NONE = 0x0,
	FWS_VISIBLE = 0x1,
	FWS_DISABLED = 0x2,
	FWS_HASTITLE = 0x4,
	FWS_CHILD = 0x8,
	FWS_TOPMOST = 0x10,
	FWS_MOVEFRAME = 0x20,
	FWS_NOMOUSEEVENT = 0x40,
	FWS_KEYEVENT = 0x80,
	FWS_FIXED = 0x100,
	FWS_NODBLCLICK = 0x200,
	FWS_HOVER = 0x400,
	FWS_NOWHEELEVENT = 0x800,
};

class FrWnd : public IObject, public FrCmdTarget {
	friend class FrWndManager;
public:
	virtual ~FrWnd();
	virtual bool Close(bool bFade);
	void SetClientRect(const WRect& rect);
	void DoFadeProcess(float deltaTime, bool bExtend);
	virtual bool Create(const char* lpszWindowText, const char* lpszWindowName, FrWndManager* pManager, unsigned int dwStyle, const WRect& rect, FrWnd* pParentWnd);
	virtual void PreCreateWindow(FrWndManager* pManager, unsigned dwStyle, const WRect& rect, FrWnd* pParentWnd);
	virtual void MoveWindow(const WPoint& pos);
	void SetRect(const WRect& rect);
	void SetAlpha2ToChild(float a);
	void FindNextTopFocus(bool bForce);
	virtual void Enable(bool bEnable);
	virtual void SetVisible(bool visible);
	virtual void OnDraw();
	virtual void OnProc(float);
	virtual void OnResize();
	virtual void OnMouseMove(const WPoint& mousePos);
	virtual bool OnLButtonUp(const WPoint& mousePos);
	virtual bool OnLButtonDown(const WPoint& mousePos);
	virtual bool OnRButtonUp(const WPoint& mousePos);
	virtual bool OnRButtonDown(const WPoint& mousePos);
	virtual void OnDblClick(const WPoint& mousePos);
	virtual void OnWheel(FrInputState& inputState);
	virtual const char* OnSelectText(const FrInputState*);
	virtual void OnKeyFocus(CChatMsg*);
	virtual void OnSetCursor(bool bInClient, const WPoint& mousePos);
	virtual void EnableKeyFocus(FrInputState&);
	virtual void SetIconRect(const WRect&);
	bool SendCmdToOwnerTarget(eFrCmd cmd, int var1, sFRESH_HANDLER* pHandler);
	void GetClientRect(WRect& rect) const;
	void SetToolTipText(const std::string& text);
	void CheckHover(float deltaTime, bool bInClient, bool& hoverChecked);
	void EnableHover(bool enable, float time);
	void SetWheelEvent(bool enable);
	bool IsViewFocused();
	bool IsTopFocus() const;
	void SetElementFadeOut(bool bOut);
	void SetFadeout(bool bOut);
	void RemoveWindow(FrWnd* pWnd);
	void DestroyChild();
	FrWnd* FindChild(const FrWnd* pWnd);
	FrWnd* FindChild(const char* lpszWindowText);
	FrWnd* FindChildByName(const char* lpszWindowName);
	FrWnd* FindChildByStyle(unsigned style);
	FrWnd* FindChildForm(const char* lpszWindowName);
	void EnumerateChildWindow(bool (*callback)(FrWnd*, void*), void* param);
	void CloseChild(FrWnd* pChildWnd, int bFade, bool force);
	void CloseChildForm(int bFade, bool force);
	void OnDisplay(bool checkTopmost, bool drawChild, bool drawOneself);
	void SetTopmost(bool topmost);
	void ResetViewFocus(FrWnd* pWndStop);
	FrWnd* FindViewFocused(bool enabled_visible);
	void DoFadeDisplay();
	bool SetKeyFocus(bool resetPrevImeData);
	bool ResetKeyFocus();
	bool SetCapture();
	FrWnd* GetCapture() const;
	bool ReleaseCapture();
	void SetCursor(int hCursor);
	int GetCursor() const;
	void MoveCursor(const char* name);
	void SetPushSound(const char* name);
	char PlayPushSound();
	void SetWheelFocus();
	WRect* GetRect();
	bool IsEnabled() const;
	bool IsVisible() const;
	void SetFixed(bool bFixed);
	void SetAlpha(float alpha);
	void HidePrivacy(bool bPrivacy);
	bool HasKeyFocus() const;
	FrScrollBar* GetScrollBar() const;
	void SetKeyEvent(bool bEnabled);
	void UseDblClick(bool bEnabled);
	FrWndManager* WndManager() const;
	float GetAlpha() const;
	const WFlags& GetStyle() const;
	bool SendCmdToOwnerTarget(FrCmdTarget* pCmdTarget, eFrCmd cmd, int var1, sFRESH_HANDLER* pHandler);
	FrGraphicInterface* GDI() const;
	FrEmoticon* Emo() const;
	FrWnd* GetParent() const;
	void SetOwner(FrCmdTarget* pOwner);
	bool IsChild(const FrWnd* pWnd) const;
	void ClientToScreen(WRect& rect) const;
	void ClientToScreen(WPoint& point) const;
	void ScreenToClient(WRect& rect) const;
	void ScreenToClient(WPoint& point) const;
	void SetWinText(const char* lpszText);
	void GetWinText(char* lpszTextBuf, unsigned nBuffMax) const;
	void GetWinText(std::string& outText) const;
	unsigned int GetWinTextLength() const;
	void SetWindowName(const char* lpszName);
	void GetWindowName(char* lpszTextBuf, unsigned nBuffMax) const;
	void GetWindowName(std::string& outName) const;
	unsigned int GetWindowNameLength() const;
	void SetFixTooltipWnd(const WPoint& pos);
	void SetUnFixToolTipWnd();
	int IsFixedToolTip() const;
	const WPoint& GetToolTipWndPos() const;
	void SetToolTipFrameStyle(unsigned int style);
	unsigned int GetToolTipFrameStyle() const;

	static void CreateToolTip(FrWndManager* pManager);
	static FrToolTip* ToolTip();
	static void DestroyToolTip();

private:
	static FrToolTip* m_pToolTip;

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
	std::string m_wndToolTip;
	sToolTipData* m_pToolTipData;
};

#pragma once
#include <list>
#include <string>


#include "bitmap.h"
#include "frguiitem.h"
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
	explicit FrWndManager(FrElementDoc* pDoc);
	~FrWndManager();
	FrDesktop* GetDesktop() const;
	FrGraphicInterface* GetGDI() const;
	bool IsKeyExclusive() const;
	FrScrollBar* GetWheelFocus() const;
	FrEmoticon* GetEmoticon() const;
	FrWnd* GetKeyFocused() const;
	const WPoint& GetMousePos() const;
	const char* GetLayoutID() const;
	const std::list<FrWnd*>& GetTopWndList() const;
	void ResetKey();
	bool HidePrivacy() const;
	unsigned& RefIndex();
	void SetCursor(int hCursor);
	int GetCursor() const;
	FrWnd* GetCapture() const;
	void HidePrivacy(bool hide);
	void CreateToolTip();
	void DestroyToolTip();
	void CloseWindow(FrWnd* pWnd, bool bFade);
	void CloseForm(bool bFade);
	void CloseLayout();
	FrElementDoc* GetDocument();
	const Bitmap* GetBitmap(const char* resource, const char* id);
	bool IsValidWindow(FrWnd* pWnd);
	void SetWheelFocus(FrScrollBar* pScrBar);
	void MoveCursor(FrWnd* pParent, const char* name);
	bool SetCapture(FrWnd* pWnd);
	bool ReleaseCapture(FrWnd* pWnd);
	void ConfineRect(WRect& dr);
	float PrintText(const WPoint* pos, unsigned align, const char* text, float limit, unsigned emoDiffuse);
	float PrintText11(const WPoint* pos, unsigned align, const char* text, float limit, unsigned emoDiffuse);
	float GetTextWidth(const char* text);
	void IME_ShowCandWindow(FrEdit* pFocusedEdit, const WPoint& caretPos);
	void CheckSystemStatus();
	FrWnd* DoCreate(FrGuiItem* item, FrWndManager* pManager, FrWnd* pParent, FrCmdTarget* pOwner);
	bool SetKeyFocus(FrWnd* pWnd, bool resetPrevImeData);
	bool CanGetKeyFocus(FrWnd* pWnd);
	WPoint GetCreatePosition(const WSize& rectSize);
	void SetExclusiveKey(bool set);
	bool Init(const char* wallPaper, bool exclusiveKey);
	void Display(bool drawDesktop);
	void ResetTopFocus();
	bool CreateLayout(const char* layout, FrCmdTarget* owner, bool firstTime);
	bool DeleteTopmostWindow(FrWnd* pWnd);
	bool ResetKeyFocus(FrWnd* pWnd);
	void RemoveFade();
	FrWnd* GetModalForm();
	FrDesktop* HandOverViewFocus();
	bool HasEscKeyWindow();
	bool HasValidWindow();
	bool AddTopmostWindow(FrWnd* pWnd);
	bool MoveKeyFocusToNext(bool resetPrevImeData);
	void ProcessHotKey();
	void ProcessKey(FrWnd* pOldKeyFocused);
	void Process(float deltaTime);

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

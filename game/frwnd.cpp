#include "frwnd.h"

#include "frtooltip.h"
#include "frscrollbar.h"
#include "frwndmanager.h"
#include "frdesktop.h"
#include "frform.h"
#include "woverlay.h"
#include "wview.h"

FrToolTip* FrWnd::m_pToolTip = nullptr;

FrWnd::~FrWnd() {
	if (this->m_pWndManager) {
		this->m_pWndManager->DeleteTopmostWindow(this);
		this->m_pWndManager->ReleaseCapture(this);
	}
	this->DestroyChild();
	if (this->m_pToolTipData) {
		delete this->m_pToolTipData;
		this->m_pToolTipData = nullptr;
	}
};

WRect* FrWnd::GetRect() {
	return &this->m_rect;
}

void FrWnd::Enable(bool bEnable) {
	this->m_dwStyle.Turn(FWS_DISABLED, !bEnable);
}

bool FrWnd::IsEnabled() const {
	return !this->m_dwStyle.GetFlag(FWS_DISABLED);
}

bool FrWnd::IsVisible() const {
	return this->m_dwStyle.GetFlag(FWS_VISIBLE);
}

void FrWnd::SetFixed(bool bFixed) {
	this->m_dwStyle.Turn(FWS_FIXED, bFixed);
}

void FrWnd::OnResize() {
}

bool FrWnd::OnRButtonUp(const WPoint& mousePos) {
	return false;
}

bool FrWnd::OnRButtonDown(const WPoint& mousePos) {
	return false;
}

void FrWnd::OnDblClick(const WPoint& mousePos) {
}

void FrWnd::OnWheel(FrInputState& inputState) {
}

const char* FrWnd::OnSelectText(const FrInputState* inputState) {
	return nullptr;
}

void FrWnd::OnKeyFocus(CChatMsg*) {
}

void FrWnd::EnableKeyFocus(FrInputState& inputState) {
	this->m_nFlags.Enable(FWF_KEYFOCUS);
}

void FrWnd::SetAlpha(float alpha) {
	this->m_wndAlpha = alpha;
}

void FrWnd::HidePrivacy(bool bPrivacy) {
	this->m_nFlags.Turn(FWF_PRIVACY, bPrivacy);
}

bool FrWnd::HasKeyFocus() const {
	return this->m_nFlags.GetFlag(FWF_KEYFOCUS);
}

FrScrollBar* FrWnd::GetScrollBar() const {
	return this->m_pScrBar;
}

void FrWnd::SetKeyEvent(bool bEnabled) {
	this->m_dwStyle.Turn(FWS_KEYEVENT, bEnabled);
}

void FrWnd::UseDblClick(bool bEnabled) {
	this->m_dwStyle.Turn(FWS_NODBLCLICK, !bEnabled);
}

FrWndManager* FrWnd::WndManager() const {
	return this->m_pWndManager;
}

float FrWnd::GetAlpha() const {
	return this->m_wndAlpha;
}

const WFlags& FrWnd::GetStyle() const {
	return this->m_dwStyle;
}

void FrWnd::OnMouseMove(const WPoint& mousePos) {
}

bool FrWnd::OnLButtonUp(const WPoint& mousePos) {
	return false;
}

bool FrWnd::OnLButtonDown(const WPoint& mousePos) {
	return false;
}

void FrWnd::OnSetCursor(bool bInClient, const WPoint &mousePos) {
}

void FrWnd::SetIconRect(const WRect& rect) {
}

void FrWnd::CreateToolTip(FrWndManager* pManager) {
	if (!m_pToolTip) {
		m_pToolTip = new FrToolTip(pManager);
	}
}

FrToolTip* FrWnd::ToolTip() {
	return m_pToolTip;
}

void FrWnd::DestroyToolTip() {
	delete m_pToolTip;
	m_pToolTip = nullptr;
}

bool FrWnd::SendCmdToOwnerTarget(FrCmdTarget* pCmdTarget, eFrCmd cmd, int var1, sFRESH_HANDLER* pHandler) {
	if (!pCmdTarget) {
		return true;
	}

	std::string fullname;
	if (this == this->m_pWndManager->m_pDesktop) {
		fullname = this->m_pWndManager->m_layoutID;
	} else if (this == pCmdTarget) {
		fullname = ".";
	} else {
		fullname = this->m_wndName;
		for (auto* i = this->m_pParentWnd; i; i = i->m_pParentWnd) {
			if (i == this->m_pWndManager->m_pDesktop) {
				break;
			}
			if (i == pCmdTarget) {
				return pCmdTarget->OnFreshMsg(fullname.c_str(), cmd, var1, pHandler);
			}
			fullname = i->m_wndName + "." + fullname;
		}
		fullname = this->m_pWndManager->m_layoutID + "." + fullname;
	}
	return pCmdTarget->OnFreshMsg(fullname.c_str(), cmd, var1, pHandler);
}

FrGraphicInterface* FrWnd::GDI() const {
	return this->m_pWndManager->m_pDevice;
}

FrEmoticon* FrWnd::Emo() const {
	return this->m_pWndManager->m_pEmoticon;
}

FrWnd* FrWnd::GetParent() const {
	return this->m_pParentWnd;
}

void FrWnd::SetOwner(FrCmdTarget* pOwner) {
	this->m_pOwner = pOwner;
}

bool FrWnd::IsChild(const FrWnd* pWnd) const {
	FrWnd* wnd = this->m_pParentWnd;
	if (!wnd) {
		return false;
	}
	while (wnd != pWnd) {
		wnd = wnd->m_pParentWnd;
		if (!wnd) {
			return false;
		}
	}
	return true;
}

void FrWnd::ClientToScreen(WRect& rect) const {
	for (FrWnd* i = this->m_pParentWnd; i; i = i->m_pParentWnd) {
		rect.x += i->m_rect.x;
		rect.y += i->m_rect.y;
	}
}

void FrWnd::ClientToScreen(WPoint& point) const {
	for (FrWnd* i = this->m_pParentWnd; i; i = i->m_pParentWnd) {
		point.x += i->m_rect.x;
		point.y += i->m_rect.y;
	}
}

void FrWnd::ScreenToClient(WRect& rect) const {
	for (FrWnd* i = this->m_pParentWnd; i; i = i->m_pParentWnd) {
		rect.x -= i->m_rect.x;
		rect.y -= i->m_rect.y;
	}
}

void FrWnd::ScreenToClient(WPoint& point) const {
	for (FrWnd* i = this->m_pParentWnd; i; i = i->m_pParentWnd) {
		point.x -= i->m_rect.x;
		point.y -= i->m_rect.y;
	}
}

void FrWnd::SetWinText(const char* lpszText) {
	if (lpszText) {
		this->m_wndText = lpszText;
	}
}

void FrWnd::GetWinText(char* lpszTextBuf, unsigned int nBuffMax) const {
	strcpy_s(lpszTextBuf, nBuffMax, this->m_wndText.c_str());
}

void FrWnd::GetWinText(std::string& outText) const {
	outText = this->m_wndText;
}

unsigned int FrWnd::GetWinTextLength() const {
	return this->m_wndText.size();
}

void FrWnd::SetWindowName(const char* lpszName) {
	if (lpszName) {
		this->m_wndName = lpszName;
	}
}

void FrWnd::GetWindowName(char* lpszTextBuf, unsigned int nBuffMax) const {
	strcpy_s(lpszTextBuf, nBuffMax, this->m_wndName.c_str());
}

void FrWnd::GetWindowName(std::string& outName) const {
	outName = this->m_wndName;
}

unsigned int FrWnd::GetWindowNameLength() const {
	return this->m_wndName.size();
}

void FrWnd::SetFixTooltipWnd(const WPoint& pos) {
	if (this->m_pToolTipData) {
		this->m_pToolTipData->bFixWnd = 1;
		this->m_pToolTipData->posFixWnd = pos;
	}
}

void FrWnd::SetUnFixToolTipWnd() {
	if (this->m_pToolTipData) {
		this->m_pToolTipData->bFixWnd = 0;
	}
}

int FrWnd::IsFixedToolTip() const {
	return this->m_pToolTipData ? this->m_pToolTipData->bFixWnd : 0;
}

const WPoint& FrWnd::GetToolTipWndPos() const {
	return this->m_pToolTipData && this->m_pToolTipData->bFixWnd ? this->m_pToolTipData->posFixWnd : this->m_pWndManager->m_istate.mousePos;
}

void FrWnd::SetToolTipFrameStyle(unsigned int style) {
	if (this->m_pToolTipData) {
		this->m_pToolTipData->style = style;
	}
}

unsigned int FrWnd::GetToolTipFrameStyle() const {
	return this->m_pToolTipData ? this->m_pToolTipData->style : 0;
}

void FrWnd::OnDraw() {
}

void FrWnd::OnProc(float deltaTime) {
}

void FrWnd::DoFadeDisplay() {
	float a = 1.0f - this->m_fadeTime;
	WRect box;
	box.x = a * this->m_iconRect.x + this->m_rect.x * this->m_fadeTime;
	box.y = a * this->m_iconRect.y + this->m_rect.y * this->m_fadeTime;
	box.w = a * this->m_iconRect.w + this->m_rect.w * this->m_fadeTime;
	box.h = a * this->m_iconRect.h + this->m_rect.h * this->m_fadeTime;
	WOverlay::DrawLineBox(g_view, box, 0, 0xFFFFFFFF);
}

bool FrWnd::SetKeyFocus(bool resetPrevImeData) {
	return this->m_pWndManager->SetKeyFocus(this, resetPrevImeData);
}

bool FrWnd::ResetKeyFocus() {
	return this->m_pWndManager->ResetKeyFocus(this);
}

bool FrWnd::SetCapture() {
	return this->m_pWndManager->SetCapture(this);
}

FrWnd* FrWnd::GetCapture() const {
	return this->m_pWndManager->m_pCaptured;
}

bool FrWnd::ReleaseCapture() {
	return this->m_pWndManager->ReleaseCapture(this);
}

void FrWnd::SetCursor(int hCursor) {
	this->m_pWndManager->m_cursorIndex = hCursor;
}

int FrWnd::GetCursor() const {
	return this->m_pWndManager->m_cursorIndex;
}

void FrWnd::MoveCursor(const char* name) {
	auto form = this->m_pWndManager->GetModalForm();
	if (!form || this == form) {
		this->m_pWndManager->MoveCursor(this, name);
	}
}

void FrWnd::SetPushSound(const char* name) {
	this->m_szPushSound = name;
}

char FrWnd::PlayPushSound() {
	// TODO: Audio
}

void FrWnd::SetWheelFocus() {
	auto *scrollBarThis = dynamic_cast<FrScrollBar*>(this);
	this->m_pWndManager->SetWheelFocus(scrollBarThis? scrollBarThis : this->m_pScrBar);
}

void FrWnd::PreCreateWindow(FrWndManager* pManager, unsigned int dwStyle, const WRect& rect, FrWnd* pParentWnd) {
	this->m_pWndManager = pManager;
	this->m_dwStyle.Set(dwStyle);
	this->m_rect = rect;
	this->m_pParentWnd = pParentWnd;
}

bool FrWnd::SendCmdToOwnerTarget(eFrCmd cmd, int var1, sFRESH_HANDLER* pHandler) {
	return this->m_nFlags.GetFlag(FWF_DESTROY) ? false : this->SendCmdToOwnerTarget(this->m_pOwner, cmd, var1, pHandler);
}

void FrWnd::GetClientRect(WRect& rect) const {
	rect = this->m_rect;
	for (FrWnd* i = this->m_pParentWnd; i; i = i->m_pParentWnd) {
		rect.x -= i->m_rect.x;
		rect.y -= i->m_rect.y;
	}
}

void FrWnd::SetToolTipText(const std::string &text) {
	this->m_wndToolTip = text;
	this->m_nFlags.Enable(FWF_TOOLTIPS);
	if (!this->m_pToolTipData) {
		this->m_pToolTipData = new sToolTipData();
		this->m_pToolTipData->style = 0;
		this->m_pToolTipData->bFixWnd = 0;
		this->m_pToolTipData->posFixWnd.x = 0.0;
		this->m_pToolTipData->posFixWnd.y = 0.0;
	}
}

void FrWnd::CheckHover(float deltaTime, bool bInClient, bool& hoverChecked) {
	if (bInClient) {
		if (!hoverChecked) {
			hoverChecked = true;
			if (!this->m_hoverOn) {
				this->m_accHoverTime = deltaTime + this->m_accHoverTime;
				if (this->m_accHoverTime > this->m_hoverTime)
				{
					this->m_accHoverTime = 0.0;
					this->m_hoverOn = true;
					this->SendCmdToOwnerTarget(FRCMD_HOVERON, reinterpret_cast<int>(this), nullptr);
				}
			}
		}
	} else {
		this->m_accHoverTime = 0.0;
		if (this->m_hoverOn) {
			this->m_hoverOn = false;
			if (!this->m_nFlags.GetFlag(FWF_DESTROY)) {
				this->SendCmdToOwnerTarget(this->m_pOwner, FRCMD_HOVEROFF, reinterpret_cast<int>(this), nullptr);
			}
		}
	}
}

void FrWnd::SetVisible(bool visible) {
	if (!visible) {
		if (this->m_pWndManager) {
			this->m_pWndManager->ReleaseCapture(this);
		}
	}

	this->m_dwStyle.Turn(FWS_VISIBLE, visible);
}

void FrWnd::EnableHover(bool enable, float time) {
	this->m_dwStyle.Turn(FWS_HOVER, enable);
	this->m_hoverTime = time;
	this->m_accHoverTime = 0.0;
}

void FrWnd::SetWheelEvent(bool enable) {
	for (auto *i = this; i != nullptr; i = i->m_pScrBar) {
		i->m_dwStyle.Turn(FWS_NOWHEELEVENT, !enable);
	}
}

bool FrWnd::IsViewFocused() {
	for (auto* i = this; i != nullptr; i = i->m_pParentWnd) {
		if (i->m_nFlags.GetFlag(FWF_VIEWFOCUS) || i->m_dwStyle.GetFlag(FWS_FIXED)) {
			return true;
		}

		// Stop at FrForm boundary.
		if (dynamic_cast<FrForm*>(this)) {
			break;
		}
	}

	return false;
}

bool FrWnd::IsTopFocus() const {
	return this->m_nFlags.GetFlag(FWF_TOPFOCUS) && this->m_dwStyle.GetFlag(FWS_TOPMOST);
}

void FrWnd::SetElementFadeOut(bool bOut) {
	if (this->m_nFlags.GetFlag(FWF_FADING_EX)) {
		this->m_nFlags.Enable(FWF_FADING_EX | FWF_FADEELEMENT);
		if (bOut) {
			this->m_nFlags.Enable(FWF_FADEOUT_EX);
			this->m_fadeTime = 1.0;
		} else {
			this->m_nFlags.Disable(FWF_FADEOUT_EX);
			this->m_fadeTime = 0.0;
		}
	}
}

void FrWnd::SetFadeout(bool bOut) {
	if (this->m_nFlags.GetFlag(FWF_FADING_EX)) {
		this->m_nFlags.Enable(FWF_FADING_EX);
		if (bOut) {
			this->m_nFlags.Enable(FWF_FADEOUT_EX);
			this->m_fadeTime = 1.0;
		}
		else {
			this->m_nFlags.Disable(FWF_FADEOUT_EX);
			this->m_fadeTime = 0.0;
		}
	}
}

void FrWnd::RemoveWindow(FrWnd* pWnd) {
	this->m_childList.remove(pWnd);
}

void FrWnd::DestroyChild() {
	this->m_childList.clear();
}

FrWnd* FrWnd::FindChild(const FrWnd* pWnd) {
	for (auto& i : this->m_childList) {
		if (i == pWnd) {
			return i;
		}
		auto *result = i->FindChild(pWnd);
		if (result != nullptr) {
			return result;
		}
	}
	return nullptr;
}

FrWnd* FrWnd::FindChild(const char* lpszWindowText) {
	for (auto& i : this->m_childList) {
		if (i->m_wndText == lpszWindowText) {
			return i;
		}
	}
	return nullptr;
}

FrWnd* FrWnd::FindChildByName(const char* lpszWindowName) {
	for (auto& i : this->m_childList) {
		if (i->m_wndName == lpszWindowName) {
			return i;
		}
	}
	return nullptr;
}

FrWnd* FrWnd::FindChildByStyle(unsigned int style) {
	for (auto& i : this->m_childList) {
		if (i->m_dwStyle.GetFlag(style)) {
			return i;
		}
	}
	return nullptr;
}

FrWnd* FrWnd::FindChildForm(const char* lpszWindowName) {
	for (auto& i : this->m_childList) {
		if (dynamic_cast<FrForm*>(i) == nullptr) {
			continue;
		}
		if (i->m_wndName == lpszWindowName) {
			return i;
		}
	}
	return nullptr;
}

void FrWnd::EnumerateChildWindow(bool(*callback)(FrWnd*, void*), void* param) {
	for (auto& i : this->m_childList) {
		if (!callback(i, param)) {
			break;
		}
	}
}

void FrWnd::CloseChild(FrWnd* pChildWnd, int bFade, bool force) {
	for (auto& i : this->m_childList) {
		if (pChildWnd == nullptr || i == pChildWnd) {
			if (force || !i->m_dwStyle.GetFlag(FWS_FIXED)) {
				i->Close(bFade);
			}
		}
	}
}

void FrWnd::CloseChildForm(int bFade, bool force) {
	for (auto& i : this->m_childList) {
		if (dynamic_cast<FrForm*>(i)) {
			if (force || !i->m_dwStyle.GetFlag(FWS_FIXED)) {
				i->Close(bFade);
			}
		}
	}
}

void FrWnd::OnDisplay(bool checkTopmost, bool drawChild, bool drawOneself) {
	if (checkTopmost && this->m_dwStyle.GetFlag(FWS_TOPMOST)) {
		return;
	}
	if (!this->m_nFlags.GetFlag(FWF_INITED) || !this->m_dwStyle.GetFlag(FWF_TOOLTIPS) && !this->m_nFlags.GetFlag(FWF_FADING_EX) && !this->m_nFlags.GetFlag(FWF_DESTROY)) {
		return;
	}
	if (this->m_nFlags.GetFlag(FWF_FADING)) {
		this->DoFadeDisplay();
	} else {
		if (drawOneself) {
			this->OnDraw();
		} if (drawChild) {
			for (auto *i : this->m_childList) {
				if (this->m_nFlags.GetFlag(FWF_FADING)) {
					i->m_wndAlpha = this->m_wndAlpha;
				}
				i->OnDisplay(true, true, true);
			}
		}
	}
}

void FrWnd::SetTopmost(bool topmost) {
	if (topmost) {
		this->m_dwStyle.Enable(FWS_TOPMOST);
		this->m_pWndManager->AddTopmostWindow(this);
	} else {
		this->m_dwStyle.Disable(FWS_TOPMOST);
		this->m_pWndManager->DeleteTopmostWindow(this);
		for (auto *i : this->m_childList) {
			i->SetTopmost(false);
		}
	}
}

void FrWnd::ResetViewFocus(FrWnd* pWndStop) {
	if (this != pWndStop) {
		for (auto *i : this->m_childList) {
			i->ResetViewFocus(pWndStop);
			i->m_pWndManager->ReleaseCapture(i);
		}
		this->m_nFlags.Disable(FWF_VIEWFOCUS);
	}
}

FrWnd* FrWnd::FindViewFocused(bool enabled_visible) {
	for (auto *i : this->m_childList) {
		if (!i->m_dwStyle.GetFlag(FWS_TOPMOST)) {
			if (i->m_nFlags.GetFlag(FWF_VIEWFOCUS) && (!enabled_visible || !i->m_dwStyle.GetFlag(FWS_DISABLED) && i->m_dwStyle.GetFlag(FWS_VISIBLE))) {
				return i;
			}
		} else if (i->m_dwStyle.GetFlag(FWF_TOPFOCUS)) {
			return i;
		}
	}
	return nullptr;
}

void FrWnd::MoveWindow(const WPoint& pos) {
	WPoint dest, step;
	step.x = pos.x - this->m_rect.x;
	step.y = pos.y - this->m_rect.y;
	this->m_rect.x = pos.x;
	this->m_rect.y = pos.y;
	for (auto *i : this->m_childList) {
		// Skip forms.
		if (dynamic_cast<FrForm*>(i)) {
			continue;
		}
		dest.x = i->m_rect.x + step.x;
		dest.y = i->m_rect.y + step.y;
		i->MoveWindow(dest);
	}
}

void FrWnd::SetRect(const WRect& rect) {
	WPoint pos;
	pos.x = rect.x;
	pos.y = rect.y;
	this->MoveWindow(pos);
	this->m_rect.w = rect.w;
	this->m_rect.h = rect.h;
	this->OnResize();
	for (auto *i : this->m_childList) {
		i->OnResize();
	}
}

void FrWnd::SetAlpha2ToChild(float a) {
	this->m_wndAlpha2 = a;
	for (auto *i : this->m_childList) {
		i->SetAlpha2ToChild(a);
	}
}

void FrWnd::FindNextTopFocus(bool bForce) {
	if (this->m_dwStyle.GetFlag(FWS_TOPMOST)) {
		this->m_pWndManager->ResetTopFocus();
		this->m_nFlags.Enable(FWF_TOPFOCUS);
		if (!this->m_nFlags.GetFlag(FWF_FADING_EX) || bForce) {
			auto* form = dynamic_cast<FrForm*>(this);
			if (form) {
				// TODO: call SetFrameCaptionFocus
				//form->SetFrameCaptionFocus(true);
			}
			this->SetAlpha2ToChild(1.0);
		}
	}
}

bool FrWnd::Close(bool bFade) {
	if (!this->m_nFlags.GetFlag(FWF_DESTROY)) {
		if (bFade) {
			this->m_nFlags.Enable(FWF_FADEOUT | FWF_FADING);
		} else {
			this->m_nFlags.Disable(FWF_FADING);
			this->m_nFlags.Enable(FWF_DESTROY);
		}
		if (this->m_pWndManager) {
			this->m_pWndManager->ResetKeyFocus(this);
		}
	}
	if (!this->m_nFlags.GetFlag(FWF_KEYFOCUS)) {
		return true;
	}
	this->m_dwStyle.Disable(FWS_TOPMOST);
	this->m_pWndManager->DeleteTopmostWindow(this);
	for (auto *i : this->m_childList) {
		i->SetTopmost(false);
	}
	return true;
}

void FrWnd::SetClientRect(const WRect& rect) {
	if (this->m_pParentWnd) {
		WRect r = rect;
		for (FrWnd* i = this->m_pParentWnd; i != nullptr; i = i->m_pParentWnd) {
			r.x += i->m_rect.x;
			r.y += i->m_rect.y;
		};
		this->SetRect(r);
	} else {
		this->SetRect(rect);
	}
}

void FrWnd::DoFadeProcess(float deltaTime, bool bExtend) {
	if (bExtend) {
		if (this->m_nFlags.GetFlag(FWF_FADEOUT_EX)) {
			this->m_fadeTime -= deltaTime * 5.0f;
			if (this->m_fadeTime < 0.0) {
				this->m_fadeTime = 0.0;
				this->m_nFlags.Disable(FWF_FADING_EX);
				if (this->m_nFlags.GetFlag(FWF_FADEELEMENT)) {
					this->m_nFlags.Disable(FWF_FADEELEMENT);
					this->m_dwStyle.Disable(FWS_VISIBLE);
				} else {
					this->Close(false);
				}
			}
		} else {
			if (this->m_nFlags.GetFlag(FWF_FADEELEMENT)) {
				this->m_dwStyle.Enable(FWS_VISIBLE);
			}
			this->m_fadeTime += deltaTime * 5.0f;
			if (this->m_fadeTime > 0.9f) {
				this->m_fadeTime = 1.0;
				this->m_nFlags.Disable(FWF_FADING_EX);
				if (this->m_nFlags.GetFlag(FWF_FADEELEMENT)) {
					this->m_nFlags.Disable(FWF_FADEELEMENT);
				}
			}
		}
		for (auto *i : this->m_childList) {
			i->SetAlpha2ToChild(this->m_wndAlpha2);
		}
	} else {
		if (this->m_nFlags.GetFlag(FWF_FADEOUT)) {
			this->m_fadeTime -= deltaTime * 5.0f;
			if (this->m_fadeTime < 0.0) {
				this->m_fadeTime = 0.0;
				if (this->m_nFlags.GetFlag(FWF_FADEELEMENT)) {
					this->m_nFlags.Disable(FWF_FADEELEMENT);
					this->m_dwStyle.Disable(FWS_VISIBLE);
				} else {
					this->m_nFlags.Enable(FWF_DESTROY);
				}
			}
		} else {
			if (this->m_nFlags.GetFlag(FWF_FADEELEMENT)) {
				this->m_dwStyle.Enable(FWS_VISIBLE);
			}
			this->m_fadeTime += deltaTime * 5.0f;
			if (this->m_fadeTime > 0.9f) {
				this->m_fadeTime = 1.0;
				this->m_nFlags.Disable(FWF_FADING);
				if (this->m_nFlags.GetFlag(FWF_FADEELEMENT)) {
					this->m_nFlags.Disable(FWF_FADEELEMENT);
				}
			}
		}
	}
}

bool FrWnd::Create(const char* lpszWindowText, const char* lpszWindowName, FrWndManager* pManager, unsigned int dwStyle, const WRect& rect, FrWnd* pParentWnd) {
	this->SetWinText(lpszWindowText);
	this->SetWindowName(lpszWindowName);
	this->PreCreateWindow(pManager, dwStyle, rect, pParentWnd);
	this->m_dwStyle.Enable(FWS_NOWHEELEVENT);
	this->m_nRefID = pManager->m_nRefIndex++;
	if (this->m_dwStyle.GetFlag(FWS_TOPMOST)) {
		pManager->AddTopmostWindow(this);
	}
	if (pParentWnd) {
		pParentWnd->m_childList.push_back(this);
		this->m_rect.x += pParentWnd->m_rect.x;
		this->m_rect.y += pParentWnd->m_rect.y;
	}
	this->m_dblClickTimeout = -100.0;
	this->m_dblClicked = false;
	return true;
}

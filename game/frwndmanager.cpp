#include "frwndmanager.h"

#include "bitmap.h"
#include "cchatmsg.h"
#include "frcursor.h"
#include "frdesktop.h"
#include "frelementdoc.h"
#include "frelementlayout.h"
#include "frform.h"
#include "frguiitem.h"
#include "frtooltip.h"
#include "frwnd.h"
#include "singleton.h"
#include "wview.h"

FrWndManager::FrWndManager(FrElementDoc* pDoc) {
	this->m_pDoc = pDoc;
	this->m_pDesktop = nullptr;
	this->m_pCursor = nullptr;
	this->m_pCaptured = nullptr;
	this->m_pDevice = nullptr;
	this->m_pEmoticon = nullptr;
	this->m_istate.im = nullptr;
	this->m_istate.keyFocused = nullptr;
	this->m_istate.wheelFocused = nullptr;
	this->m_pFocusedEdit = nullptr;
	this->m_caretPos.x = 0.0;
	this->m_hidePrivacy = false;
	this->m_caretPos.y = 0.0;
}

FrWndManager::~FrWndManager() {
	if (this->m_pDesktop) {
		delete this->m_pDesktop;
		this->m_pDesktop = nullptr;
	}
	if (this->m_pDevice) {
		// TODO: Call delete
		//delete this->m_pDevice;
		this->m_pDevice = nullptr;
	}
	if (this->m_pCursor) {
		delete this->m_pCursor;
		this->m_pCursor = nullptr;
	}
	if (this->m_pEmoticon) {
		// TODO: Call delete
		//delete this->m_pEmoticon;
		this->m_pEmoticon = nullptr;
	}
	if (this->m_exclusiveKey && WSingleton<CChatMsg>::Instance()) {
		// TODO: Call SetActive
		//WSingleton<CChatMsg>::Instance()->SetActive(false, true, true);
	}
}

FrDesktop* FrWndManager::GetDesktop() const {
	return this->m_pDesktop;
}

FrGraphicInterface* FrWndManager::GetGDI() const {
	return this->m_pDevice;
}

bool FrWndManager::IsKeyExclusive() const {
	return this->m_exclusiveKey;
}

FrScrollBar* FrWndManager::GetWheelFocus() const {
	return this->m_istate.wheelFocused;
}

FrEmoticon* FrWndManager::GetEmoticon() const {
	return this->m_pEmoticon;
}

FrWnd* FrWndManager::GetKeyFocused() const {
	return this->m_istate.keyFocused;
}

const WPoint& FrWndManager::GetMousePos() const {
	return this->m_istate.mousePos;
}

const char* FrWndManager::GetLayoutID() const {
	return this->m_layoutID.c_str();
}

const std::list<FrWnd*>& FrWndManager::GetTopWndList() const {
	return this->m_topmostList;
}

void FrWndManager::ResetKey() {
	if (this->m_istate.im) {
		// TODO: Call Reset
		//this->m_istate.im->Reset();
	}
}

bool FrWndManager::HidePrivacy() const {
	return this->m_hidePrivacy;
}

unsigned int& FrWndManager::RefIndex() {
	return this->m_nRefIndex;
}

void FrWndManager::SetCursor(int hCursor) {
	this->m_cursorIndex = hCursor;
}

int FrWndManager::GetCursor() const {
	return this->m_cursorIndex;
}

FrWnd* FrWndManager::GetCapture() const {
	return this->m_pCaptured;
}

void FrWndManager::HidePrivacy(bool hide) {
	this->m_hidePrivacy = hide;
}

void FrWndManager::CreateToolTip() {
	FrWnd::CreateToolTip(this);
}

void FrWndManager::DestroyToolTip() {
	FrWnd::DestroyToolTip();
}

void FrWndManager::CloseWindow(FrWnd* pWnd, bool bFade) {
	this->m_pDesktop->CloseChild(pWnd, bFade, false);
}

void FrWndManager::CloseForm(bool bFade) {
	this->m_pDesktop->CloseChildForm(bFade, false);
}

void FrWndManager::CloseLayout() {
	this->m_pDesktop->SendCmdToOwnerTarget(FRCMD_DESTROY, 0, 0);
	this->m_pDesktop->CloseChild(0, 0, 1);
	this->m_pDesktop->SetOwner(nullptr);
	this->m_layoutID = "";
}

FrElementDoc* FrWndManager::GetDocument() {
	return this->m_pDoc;
}

const Bitmap* FrWndManager::GetBitmap(const char* resource, const char* id) {
	if (!this->m_pDoc) {
		return nullptr;
	}
	// TODO: Call GetBitmap
	//return this->m_pDoc->GetBitmap(id ? id : "");
	return nullptr;
}

bool FrWndManager::IsValidWindow(FrWnd* pWnd) {
	if (this->m_pDesktop == pWnd) {
		return true;
	} else {
		return this->m_pDesktop->FindChild(pWnd) != nullptr;
	}
}

void FrWndManager::SetWheelFocus(FrScrollBar* pScrBar) {
	this->m_istate.wheelFocused = pScrBar;
}

void FrWndManager::MoveCursor(FrWnd* pParent, const char* name) {
	FrWnd* wnd = pParent->FindChildByName(name);
	if (wnd) {
		WPoint point;
		point.x = wnd->m_rect.w * 0.5f + wnd->m_rect.x;
		point.y = wnd->m_rect.h * 0.5f + wnd->m_rect.y;
		// TODO: Call MoveCursor
		//this->m_pCursor->MoveCursor(point);
	}
}

bool FrWndManager::SetCapture(FrWnd* pWnd) {
	if (this->m_pCaptured) {
		return false;
	}
	this->m_pCaptured = pWnd;
	return true;
}

bool FrWndManager::ReleaseCapture(FrWnd* pWnd) {
	if (this->m_pCaptured != pWnd) {
		return false;
	}
	this->m_pCaptured = nullptr;
	return true;
}

void FrWndManager::ConfineRect(WRect& dr) {
	if (dr.x < this->m_pDesktop->m_rect.x) {
		dr.x = this->m_pDesktop->m_rect.x;
	}
	if (dr.y < this->m_pDesktop->m_rect.y) {
		dr.y = this->m_pDesktop->m_rect.y;
	}
	float maxX = this->m_pDesktop->m_rect.w + this->m_pDesktop->m_rect.x - 100.0f;
	if (dr.x > maxX) {
		dr.x = maxX;
	}
	float maxY = this->m_pDesktop->m_rect.h + this->m_pDesktop->m_rect.y - 30.0f;
	if (dr.y > maxY) {
		dr.y = maxY;
	}
}

float FrWndManager::PrintText(const WPoint* pos, unsigned int align, const char* text, float limit, unsigned int emoDiffuse) {
	// TODO: Need to implement FrEmoticon, CChatMsg, FrGraphicInterface.
	abort();
}

float FrWndManager::PrintText11(const WPoint* pos, unsigned int align, const char* text, float limit, unsigned int emoDiffuse) {
	// TODO: Need to implement FrEmoticon, CChatMsg, FrGraphicInterface.
	abort();
}

float FrWndManager::GetTextWidth(const char* text) {
	if (!this->m_pDevice) {
		return 0.0;
	}
	if (this->m_pEmoticon) {
		// TODO: Call GetTextWidth
		//return this->m_pEmoticon->GetTextWidth(text);
	}
	// TODO: Call GetTextExtend
	//return this->m_pDevice->GetTextExtend(text);
	return 0.0;
}

void FrWndManager::IME_ShowCandWindow(FrEdit* pFocusedEdit, const WPoint& caretPos) {
	this->m_pFocusedEdit = pFocusedEdit;
	this->m_caretPos = caretPos;
}

void FrWndManager::CheckSystemStatus() {
	// TODO: Implement
}

FrWnd* FrWndManager::DoCreate(FrGuiItem* item, FrWndManager* pManager, FrWnd* pParent, FrCmdTarget* pOwner) {
	FrWnd* wnd; // eax
	sFrCreateSub m;
	m.pItem = item;
	m.pManager = this;
	m.pOwner = pOwner;
	m.pParent = pParent;
	switch (item->m_type)
	{
	case GI_FORM:
		// TODO: Call DoCreateItem
		//wnd = DoCreateItem<FrForm>(&m);
		break;
	case GI_STATIC:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrStatic>(&m);
		break;
	case GI_TEXTBUTTON:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrTextButton>(&m);
		break;
	case GI_EDIT:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrEdit>(&m);
		break;
	case GI_COMBOBOX:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrComboBox>(&m);
		break;
	case GI_COMBOCTLEX:
		// TODO: Call DoCreateItem
		//wnd = DoCreateItem<FrComboCtlEx>(&m);
		break;
	case GI_BUTTON:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrButton>(&m);
		break;
	case GI_FRAME:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrFrame>(&m);
		break;
	case GI_AREA:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrArea>(&m);
		break;
	case GI_LISTBOX:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrListBox>(&m);
		break;
	case GI_GAUGEBAR:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrGaugeBar>(&m);
		break;
	case GI_GAUGEBAREX:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrGaugeBarEx>(&m);
		break;
	case GI_GAUGEBARIMAGE:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrGaugeBarImage>(&m);
		break;
	case GI_VIEWER:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrViewer>(&m);
		break;
	case GI_CONTEXTMENU:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrContextMenuCtrl>(&m);
		break;
	case GI_TABBUTTON:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrTabButton>(&m);
		break;
	case GI_GROUPBOX:
		// TODO: Call DoCreateItem
		//wnd = (FrComboCtlEx*)DoCreateItem<FrGroupBox>(&m);
		break;
	case GI_MACROITEM:
		// TODO: Call DoCreateMacroItem
		//DoCreateMacroItem(&m);
		return nullptr;
	default:
		return nullptr;
	}
	if (wnd) {
		wnd->SendCmdToOwnerTarget(FRCMD_INIT, reinterpret_cast<int>(wnd), nullptr);
	}
	return wnd;
}

bool FrWndManager::SetKeyFocus(FrWnd* pWnd, bool resetPrevImeData) {
	if (pWnd) {
		if (!pWnd->m_dwStyle.GetFlag(FWS_KEYEVENT) || !pWnd->m_dwStyle.GetFlag(FWS_VISIBLE) || pWnd->m_dwStyle.GetFlag(FWS_DISABLED)) {
			return false;
		}
	}
	if (this->m_istate.keyFocused) {
		this->m_istate.keyFocused->m_nFlags.Disable(FWF_KEYFOCUS);
		this->m_istate.keyFocused->SendCmdToOwnerTarget(FRCMD_LOSTKEYFOCUS, reinterpret_cast<int>(&this->m_istate), nullptr);
	}
	if (pWnd) {
		pWnd->EnableKeyFocus(this->m_istate);
		if (resetPrevImeData && this->m_exclusiveKey) {
			std::string text;
			pWnd->GetWinText(text);
			// TODO: CChatMsg calls
			//WSingleton<CChatMsg>::Instance()->Reset();
			//WSingleton<CChatMsg>::Instance()->SetChatText(text.c_str(), false);
		}
	}
	this->m_istate.keyFocused = pWnd;
	return true;
}

bool FrWndManager::CanGetKeyFocus(FrWnd* pWnd) {
	if (pWnd) {
		if (!pWnd->m_dwStyle.GetFlag(FWS_KEYEVENT) || !pWnd->m_dwStyle.GetFlag(FWS_VISIBLE) || pWnd->m_dwStyle.GetFlag(FWS_DISABLED)) {
			return false;
		}
	}
	return true;
}

WPoint FrWndManager::GetCreatePosition(const WSize& rectSize) {
	WRect dr;
	dr.x = static_cast<float>(static_cast<int>((this->m_pDesktop->m_rect.w - rectSize.w + 1.0) * 0.5)) + this->m_pDesktop->m_rect.x;
	dr.y = static_cast<float>(static_cast<int>((this->m_pDesktop->m_rect.h - rectSize.h + 1.0) * 0.5)) + this->m_pDesktop->m_rect.y;
	dr.w = rectSize.w;
	dr.h = rectSize.h;
	this->ConfineRect(dr);
	return {dr.x, dr.y};
}

void FrWndManager::SetExclusiveKey(bool set) {
	this->m_exclusiveKey = set;
	if (WSingleton<CChatMsg>::Instance()) {
		// TODO: Call SetActive
		//WSingleton<CChatMsg>::Instance()->SetActive(set, true, false);
	}
}

bool FrWndManager::Init(const char* wallPaper, bool exclusiveKey) {
	this->m_exclusiveKey = exclusiveKey;
	this->m_cursorIndex = 0;
	this->m_istate.mouse = 0;
	this->m_istate.hoverChecked = false;
	this->m_istate.mousePos.x = 0.0;
	this->m_istate.mousePos.y = 0.0;
	this->m_istate.oldMousePos.x = 0.0;
	this->m_istate.oldMousePos.y = 0.0;
	this->m_istate.wheelDelta = 0.0;
	this->m_istate.keyFocused = nullptr;
	this->m_istate.wheelFocused = nullptr;
	this->m_istate.im = nullptr;
	this->CheckSystemStatus();
	if (!this->m_pDevice) {
		// TODO: Call new
		//this->m_pDevice = new FrGraphicInterface(g_view);
	}
	// TODO: Call Reset
	//this->m_pDevice->Reset();
	if (this->m_pDesktop) {
		delete this->m_pDesktop;
		this->m_pDesktop = nullptr;
	}
	this->m_pDesktop = new FrDesktop();
	this->m_pDesktop->m_pWndManager = this;
	// TODO: Init Desktop
	//this->m_pDesktop->Init(g_view->SCREEN_XS, g_view->SCREEN_YS);
	//this->m_pDesktop->SetWallPaper(wallPaper, 1);
	if (!this->m_pCursor) {
		this->m_pCursor = new FrCursor();
	}
	// TODO: Init Cursor
	//FrCursor::Init(this->m_pCursor);
	if (!this->m_pEmoticon) {
		// TODO: Call new
		//this->m_pEmoticon = new FrEmoticon(this);
	}
	// TODO: Init Emoticon
	//this->m_pEmoticon->Init();
	if (this->m_exclusiveKey) {
		// TODO: Call SetActive
		//WSingleton<CChatMsg>::m_pInstance->SetActive(true, true, true);
	}
	return true;
}

void FrWndManager::Display(bool drawDesktop) {
	this->m_pFocusedEdit = nullptr;
	this->m_pDesktop->OnDisplay(true, true, drawDesktop);
	for(auto *i : this->m_topmostList) {
		if (!i->m_pParentWnd || i->m_pParentWnd->m_dwStyle.GetFlag(1)) {
			i->OnDisplay(false, true, true);
		}
	}
	if (FrWnd::m_pToolTip) {
		// TODO: Call OnDisplay
		//FrWnd::ToolTip()->OnDisplay();
	}
}

void FrWndManager::ResetTopFocus() {
	for (auto *i : this->m_topmostList) {
		i->m_nFlags.Disable(0x200);
		if (i->m_wndName == "messenger_chat" && !i->m_nFlags.GetFlag(0x20)) {
			auto* form = dynamic_cast<FrForm*>(i);
			if (form) {
				// TODO: Call SetFrameCaptionFocus
				//form->SetFrameCaptionFocus(false);
			} else {
				i->SetAlpha2ToChild(0.7f);
			}
		}
	}
}

bool FrWndManager::CreateLayout(const char* layout, FrCmdTarget* owner, bool firstTime) {
	FrElementLayout* pElement = this->m_pDoc->GetLayout(layout);
	if (!pElement) {
		return false;
	}
	this->m_layoutID = layout;
	this->m_pDesktop->m_pOwner = owner;
	this->m_pDesktop->ReleaseCapture();
	// TODO: Call SetViewFocus
	//this->m_pDesktop->SetViewFocus(true);
	this->m_istate.keyFocused = nullptr;
	this->m_pDesktop->SendCmdToOwnerTarget(FRCMD_INIT, 0, nullptr);
	for (auto* i : pElement->m_guiList) {
		this->DoCreate(i, this, this->m_pDesktop, owner);
	}
	// TODO: Call SetAniBg
	//this->m_pDesktop->SetAniBg(&pElement->m_aniBg);
	this->m_pDesktop->SendCmdToOwnerTarget(FRCMD_FINISH, 0, nullptr);
	if (!firstTime) {
		this->m_state = OPENING;
		this->m_fadeoutAlpha = 0.0;
	}
	return true;
}

bool FrWndManager::DeleteTopmostWindow(FrWnd* pWnd) {
	auto it = std::find(this->m_topmostList.begin(), this->m_topmostList.end(), pWnd);
	if (it != this->m_topmostList.end()) {
		this->m_topmostList.erase(it);
		return true;
	}
	return false;
}

bool FrWndManager::ResetKeyFocus(FrWnd* pWnd) {
	if (this->m_istate.keyFocused == pWnd) {
		this->m_istate.keyFocused = nullptr;
		pWnd->m_nFlags.Disable(FWF_KEYFOCUS);
		pWnd->SendCmdToOwnerTarget(FRCMD_LOSTKEYFOCUS, reinterpret_cast<int>(&this->m_istate), nullptr);
		return true;
	}
	for (auto *i : pWnd->m_childList) {
		if (this->ResetKeyFocus(i)) {
			return true;
		}
	}
	return false;
}

void FrWndManager::RemoveFade() {
	for (auto *i : this->m_pDesktop->m_childList) {
		i->m_fadeTime = -1.0;
	}
}

FrWnd* FrWndManager::GetModalForm() {
	// TODO: implement
	return nullptr;
}

FrDesktop* FrWndManager::HandOverViewFocus() {
	// TODO: implement
	return nullptr;
}

bool FrWndManager::HasEscKeyWindow() {
	// TODO: implement
	return false;
}

bool FrWndManager::HasValidWindow() {
	// TODO: implement
	return false;
}

bool FrWndManager::AddTopmostWindow(FrWnd* pWnd) {
	// TODO: implement
	return false;
}

bool FrWndManager::MoveKeyFocusToNext(bool resetPrevImeData) {
	// TODO: implement
	return false;
}

void FrWndManager::ProcessHotKey() {
	// TODO: implement
}

void FrWndManager::ProcessKey(FrWnd* pOldKeyFocused) {
	// TODO: implement
}

void FrWndManager::Process(float deltaTime) {
	// TODO: implement
}

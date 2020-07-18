﻿#pragma once
#include <map>
#include <string>

#include "rectangle.h"

enum BGTYPE {
	BT_IMAGEFILE,
	BT_BORDERFILL,
	BT_NONE,
};

enum BUTTONPARTS {
	BUTTONPartFiller0,
	BP_PUSHBUTTON,
	BP_RADIOBUTTON,
	BP_CHECKBOX,
	BP_GROUPBOX,
	BP_USERBUTTON,
};

enum enumGuiType {
	GI_NONE,
	GI_FORM,
	GI_STATIC,
	GI_TEXTBUTTON,
	GI_EDIT,
	GI_COMBOBOX,
	GI_COMBOCTLEX,
	GI_BUTTON,
	GI_FRAME,
	GI_RESOURCE,
	GI_BITMAP,
	GI_AREA,
	GI_LISTBOX,
	GI_GAUGEBAR,
	GI_GAUGEBAREX,
	GI_GAUGEBARIMAGE,
	GI_VIEWER,
	GI_CONTEXTMENU,
	GI_TABBUTTON,
	GI_GROUPBOX,
	GI_MACROITEM,
	GI_LAST,
};

class FrWndManager;
class FrWnd;
class FrCmdTarget;

class FrGuiItem {
public:
	FrGuiItem();
	
	enumGuiType m_type = GI_NONE;
	std::string m_resource{};
	std::string m_caption{};
	std::string m_name{};
	std::map<std::string, std::string> m_param{};
	unsigned int m_flag = 0;
	Rectangle_<short> m_rect{};
};

struct sFrCreateSub {
	FrGuiItem* pItem;
	FrWndManager* pManager;
	FrWnd* pParent;
	FrCmdTarget* pOwner;
};

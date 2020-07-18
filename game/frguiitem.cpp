#include "frguiitem.h"

#include "../third_party/tinyxml/tinyxml.h"

FrGuiTID GuiType;

FrGuiTID::FrGuiTID() {
	this->m_map["NONE"] = GI_NONE;
	this->m_map["FORM"] = GI_FORM;
	this->m_map["STATIC"] = GI_STATIC;
	this->m_map["TEXTBUTTON"] = GI_TEXTBUTTON;
	this->m_map["EDIT"] = GI_EDIT;
	this->m_map["COMBOBOX"] = GI_COMBOBOX;
	this->m_map["COMBOCTLEX"] = GI_COMBOCTLEX;
	this->m_map["BUTTON"] = GI_BUTTON;
	this->m_map["FRAME"] = GI_FRAME;
	this->m_map["RESOURCE"] = GI_RESOURCE;
	this->m_map["BITMAP"] = GI_BITMAP;
	this->m_map["AREA"] = GI_AREA;
	this->m_map["LISTBOX"] = GI_LISTBOX;
	this->m_map["GAUGEBAR"] = GI_GAUGEBAR;
	this->m_map["GAUGEBAREX"] = GI_GAUGEBAREX;
	this->m_map["GAUGEBARIMAGE"] = GI_GAUGEBARIMAGE;
	this->m_map["VIEWER"] = GI_VIEWER;
	this->m_map["CONTEXTMENU"] = GI_CONTEXTMENU;
	this->m_map["TABBUTTON"] = GI_TABBUTTON;
	this->m_map["GROUPBOX"] = GI_GROUPBOX;
	this->m_map["MACROITEM"] = GI_MACROITEM;
}

FrGuiTID::~FrGuiTID() = default;

void CreateGuiItem(const TiXmlNode* pSrc, std::list<FrGuiItem*>& guiList) {
	// TODO: implement
	abort();
}

FrGuiItem::FrGuiItem() = default;

FrGuiItem::~FrGuiItem() = default;

void FrGuiItem::Init(const TiXmlNode* pSrc) {
	const TiXmlElement* pElem = pSrc->ToElement();
	std::string aType = pElem->Attribute("type");
	std::string aRect = pElem->Attribute("rect");
	std::string aPos = pElem->Attribute("pos");
	this->m_rect = {};
	if (aPos.empty()) {
		int v[4];
		sscanf_s(aRect.c_str(), "%d %d %d %d", &v[0], &v[1], &v[2], &v[3]);
		this->m_rect.tl.x = v[0];
		this->m_rect.tl.y = v[1];
		this->m_rect.br.x = v[2];
		this->m_rect.br.y = v[3];
	} else {
		int v[2];
		sscanf_s(aPos.c_str(), "%d %d", &v[0], &v[1]);
		this->m_rect.tl.x = v[0];
		this->m_rect.tl.y = v[1];
		this->m_rect.br.x = v[0] + 10;
		this->m_rect.br.y = v[1] + 10;
	}

	this->m_type = GuiType[aType];
	this->m_resource = pElem->Attribute("resource");
	this->m_name = pElem->Attribute("name");
	this->m_caption = pElem->Attribute("caption");
	pElem->Attribute("flag", reinterpret_cast<int*>(&this->m_flag));

	const TiXmlNode* pParam = pSrc->FirstChild("param");
	while (pParam) {
		const TiXmlElement* pParamElem = pParam->ToElement();
		std::string aName = pParamElem->Attribute("name");
		std::string aVar = pParamElem->Attribute("var");
		if (!aName.empty() && !aVar.empty()) {
			this->m_param[aName] = aVar;
		}
		pParam = pParam->NextSibling();
	}
}

FrGuiItemNested::FrGuiItemNested() = default;

FrGuiItemNested::~FrGuiItemNested() {
	for (auto* i : m_childList) {
		delete i;
	}
}

std::list<FrGuiItem*>& FrGuiItemNested::GetChildList() {
	return this->m_childList;
}

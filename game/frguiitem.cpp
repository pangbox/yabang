#include "frguiitem.h"

#include "../third_party/tinyxml/tinyxml.h"

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

	// TODO: Set m_type
	//this->m_type = GuiType[aType];
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

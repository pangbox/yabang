#include "frelementlayout.h"

#include "../third_party/tinyxml/tinyxml.h"

FrElementLayout::FrElementLayout()
	: FrElement(ED_LAYOUT) {}

FrElementLayout::~FrElementLayout() {
	for (auto *i : this->m_guiList) {
		delete i;
	}
}

void FrElementLayout::Init(const TiXmlNode* pSrc) {
    const TiXmlNode* base = pSrc->FirstChild("base");
	if (!base || !base->ToElement()) {
		return;
	}

	char buff[256];
	const TiXmlElement* elem = base->ToElement();
	elem->Attribute("width", &this->m_bgSize.cx);
	elem->Attribute("height", &this->m_bgSize.cy);
	std::string colorAttr = elem->Attribute("color");
	sprintf(buff, "0x%s", colorAttr.c_str());
	sscanf(buff, "%X", &this->m_bgColor);
	this->m_bgFilename = elem->Attribute("background");
	//CreateGuiItem(pSrc, &this->m_guiList);

	const TiXmlNode* aniBg = pSrc->FirstChild("anibg");
	if (!aniBg || !aniBg->ToElement()) {
		return;
	}

	int v[4];
    const TiXmlElement* aniElement = aniBg->ToElement();
    std::string rect = aniElement->Attribute("rect");
    sscanf(rect.c_str(), "%d %d %d %d", &v[0], &v[1], &v[2], &v[3]);
    this->m_aniBg.m_rect.tl.x = v[0];
    this->m_aniBg.m_rect.tl.y = v[1];
    this->m_aniBg.m_rect.br.x = v[2];
    this->m_aniBg.m_rect.br.y = v[3];
    this->m_aniBg.m_param["vel"] = aniElement->Attribute("vel");
    this->m_aniBg.m_param["img"] = aniElement->Attribute("img");
}

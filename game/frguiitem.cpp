#include "frguiitem.h"

void CreateGuiItem(const TiXmlNode* pSrc, std::list<FrGuiItem*>& guiList) {
	// TODO: implement
	abort();
}

FrGuiItem::FrGuiItem() = default;

FrGuiItem::~FrGuiItem() = default;

FrGuiItemNested::FrGuiItemNested() = default;

FrGuiItemNested::~FrGuiItemNested() {
	for (auto* i : m_childList) {
		delete i;
	}
}

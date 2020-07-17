#include "frelementdoc.h"

#include "bitmap.h"
#include "frelementlayout.h"

FrElementDoc::~FrElementDoc() {
	for (auto* element : this->m_elementList) {
		delete element;
	}
	this->m_elementList.clear();
	this->ClearBitmaps();
};

FrElementLayout* FrElementDoc::GetLayout(const std::string& resID) {
	auto it = std::find_if(this->m_elementList.begin(), this->m_elementList.end(), [&](FrElement* e) {
		return e->m_type == ED_LAYOUT && e->m_name == resID;
	});
	if (it == this->m_elementList.end()) {
		return nullptr;
	}
	return dynamic_cast<FrElementLayout*>(*it);
}

void FrElementDoc::ClearBitmaps() {
	for (auto [_, value] : this->m_mapBitmap) {
		delete value.bitmap;
	}
	this->m_mapBitmap.clear();
}

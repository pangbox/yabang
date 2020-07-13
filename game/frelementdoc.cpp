#include "frelementdoc.h"

#include "frelementlayout.h"

FrElementDoc::~FrElementDoc() {
	// TODO: Implement
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

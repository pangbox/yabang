#include "baseobject.h"

int BaseObject::m_nextId = 0;
int BaseObject::m_count = 0;
ConstructFn BaseObject::m_constructFn = nullptr;

BaseObject::BaseObject() {
	this->m_ref = 0;
	this->m_id = m_nextId++;
	++m_count;
	if (m_constructFn) {
		m_constructFn(this);
	}
}

void BaseObject::SetLeakHint(const char* hint) {
	this->m_leakHint = hint;
}

#include "iobject.h"

WRTTI IObject::m_RTTI = WRTTI("IObject", nullptr);

WRTTI::WRTTI(const char* pName, WRTTI* pBaseRtti): m_pName(pName),
                                                   m_pBaseRTTI(pBaseRtti) {}

WRTTI* WRTTI::GetBaseRTTI() const {
	return this->m_pBaseRTTI;
}

const char* WRTTI::GetName() const {
	return this->m_pName;
}

IObject::IObject() = default;

IObject::~IObject() = default;

bool IObject::IsKindOf(const WRTTI* pOther) {
    WRTTI* pSelf = GetRTTI();
    if (!pSelf) {
        return false;
    }
    while (pSelf != pOther) {
        pSelf = pSelf->m_pBaseRTTI;
        if (!pSelf) {
            return false;
        }
    }
    return true;
}

bool IObject::IsExactKindOf(const WRTTI* pOther) {
    return GetRTTI() == pOther;
}

IObject* IObject::DynamicCast(const WRTTI* pOther) {
    WRTTI* pSelf = GetRTTI();
    if (!pSelf) {
        return nullptr;
    }
    while (pSelf != pOther)
    {
        pSelf = pSelf->m_pBaseRTTI;
        if (!pSelf) {
            return nullptr;
        }
    }
    return this;
}

WRTTI* IObject::GetRTTI() {
    return &m_RTTI;
}

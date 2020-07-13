#pragma once
#include "baseobject.h"

class WResourceManager;

class WResource : public BaseObject {
	friend WResourceManager;
public:
	WResource();

protected:
	WResourceManager* m_resrcMng = nullptr;
};

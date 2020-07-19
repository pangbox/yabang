#pragma once
#include <string>

class BaseObject;

typedef void (__fastcall* ConstructFn)(BaseObject*);

class BaseObject {
public:
	BaseObject();
	virtual ~BaseObject();
	void SetLeakHint(const char* hint);

protected:
	static int m_nextId;
	static int m_count;
	static ConstructFn m_constructFn;

	int m_id = 0;
	int m_ref;
	std::string m_leakHint = "";
};

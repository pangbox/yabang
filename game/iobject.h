#pragma once

class WRTTI {
	friend class IObject;

public:
	WRTTI(const char* pName, WRTTI* pBaseRtti);
	WRTTI* GetBaseRTTI() const;
	const char* GetName() const;

private:
	const char* m_pName;
	WRTTI* m_pBaseRTTI;
};

class IObject {
public:
	IObject();
	virtual ~IObject();

	bool IsKindOf(const WRTTI* pOther);
	bool IsExactKindOf(const WRTTI* pOther);
	IObject* DynamicCast(const WRTTI* pOther);
	virtual WRTTI* GetRTTI();

private:
	static WRTTI m_RTTI;
};

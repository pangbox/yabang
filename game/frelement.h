#pragma once
#include <string>

class TiXmlNode;

enum enumElement {
	ED_NONE = 0x0,
	ED_FRAME = 0x1,
	ED_LAYOUT = 0x2,
	ED_FORM = 0x3,
	ED_BITMAP = 0x4,
	ED_ICON = 0x5,
	ED_INCLUDE = 0x6,
	ED_MACROITEM = 0x7,
	ED_LAST = 0x8,
};

class FrElement {
public:
	explicit FrElement(enumElement type);
	virtual ~FrElement();

	virtual void Init(const TiXmlNode*) = 0;

	std::string m_name;
	enumElement m_type;
};

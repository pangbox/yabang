#pragma once
#include <list>
#include <string>

#include "frelement.h"
#include "frguiitem.h"

class TiXmlNode;

class FrElementLayout : public FrElement {
public:
	~FrElementLayout();
	void Init(const TiXmlNode*) override;

	struct sSize {
		int cx, cy;
	};

	sSize m_bgSize;
	unsigned int m_bgColor;
	std::string m_bgFilename;
	std::list<FrGuiItem*> m_guiList;
	FrGuiItem m_aniBg;
};

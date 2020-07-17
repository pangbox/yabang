#pragma once
#include <list>
#include <map>
#include <string>

#include "frelementlayout.h"

class FrElement;
class Bitmap;

struct resBitmap_t {
	Bitmap* bitmap;
	int size;
	std::string name;
};

class FrElementDoc {
public:
	virtual ~FrElementDoc();
	FrElementLayout* GetLayout(const std::string& resID);
	void ClearBitmaps();

private:
	std::list<FrElement*> m_elementList;
	std::map<std::string, resBitmap_t> m_mapBitmap;
	std::string m_xmlFileName;
};

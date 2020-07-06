#pragma once
#include <map>

#include "frwnd.h"

class Bitmap;
class FrGuiItem;
class FrListItem;

class FrListBoxGreater {
public:
	bool (__fastcall* m_compare)(const void*, const void*);
};

class FrListBox : public FrWnd {
public:
	struct sSize {
		int w, h;
	};

private:
	std::list<FrListItem*> m_itemList;
	FrGuiItem* m_pItem;
	sSize m_itemSize;
	int m_colCapacity;
	int m_prevTopRow;
	std::list<FrListItem*>::iterator m_itemTop;
	std::list<FrListItem*>::iterator m_itemBottom;
	bool m_multiSelect;
	bool m_useRightButton;
	bool m_rollOver;
	FrListItem* m_pItemUnderCursor;
	FrListItem* m_pSelected;
	FrListItem* m_pPreviousSelected;
	int m_dummyCount;
	bool m_useDummy;
	std::map<int, WRect> m_noRButtonMap;
	const Bitmap* m_listBgImg;
};

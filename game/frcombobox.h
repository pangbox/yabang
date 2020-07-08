#pragma once
#include "fredit.h"
#include "frguiitem.h"

class FrButton;
class FrListBox;

class FrComboBox : public FrEdit {
public:

private:
	int m_maxListNum;
	std::list<FrLine*> m_comboList;
	std::list<FrLine*>::iterator m_selected;
	FrButton* m_pButton;
	FrGuiItem m_buttonInfo;
	FrListBox* m_pListBox;
	FrGuiItem m_listInfo;
	unsigned int m_selectColor;
};

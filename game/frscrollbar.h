#pragma once
#include "frwnd.h"

class Bitmap;

class FrScrollBar : public FrWnd {
public:

private:
	struct Frag {
		int width;
		int height;
		const Bitmap* n_img;
		const Bitmap* o_img;
	};
	
	float m_barH;
	float m_barY;
	int m_itemNum;
	float m_curTopRow;
	int m_rowCapacity;
	int m_colCapacity;
	float m_dragPrevY;
	bool m_followBottom;
	bool m_showGuide;
	bool m_atLeft;
	const Bitmap* m_frames[3];
	bool m_mouseDown;
	Frag m_frag[3];
};

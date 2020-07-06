#pragma once

class FrCursor {
public:
	enum eCursor {
		UNSELECT = 0x0,
		NORMAL = 0x0,
		ROLL_OVER = 0x1,
		SELECT = 0x2,
		WRONG = 0x3,
		UP = 0x4,
		DOWN = 0x5,
		ZOOM = 0x6,
		INFO = 0x7,
	};

private:
	int m_cursorIndex;
	bool m_bShow;
};

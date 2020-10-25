#pragma once
#include <vector>

#include "singleton.h"
#include "wmath.h"

class WOverlay;
class CGroundPropt;

class CMouseCursor : WSingleton<CMouseCursor> {
	struct sPointer {
		WOverlay* overlay;
		float w;
		float h;
		float len;
		int num;
	};

	WVector2D m_pointer;
	WVector2D m_oldPointer;
	WVector2D m_mouse;
	WVector2D m_target;
	WVector2D m_delta;
	int m_mode;
	CMouseCursor::sPointer* m_overlay;
	float m_stopDuration;
	float m_frequency;
	int m_frame;
	bool m_show;
	bool m_active;
	int m_area;
	int m_cursor;
	bool m_validButton[3];
	float m_dt;
	WRect* m_bound;
	bool m_camMode;
	float m_forceMoving;
	float m_forceFactor;
	int m_layer;
	std::vector<int> m_activeArea;
	bool m_itemWindow;
	int m_buttonDownArea[3];
	bool m_ignoreButtonDownArea;
	bool m_tempActive;
	WVector2D m_holecup;
	WRect m_groundArea;
	WVector m_ray;
	float m_rayRatio;
	bool m_autoHide;
	bool m_bArea[73];
	bool m_bShow;
	bool m_bRightButton;
	CGroundPropt* m_pGroundPt;
	bool m_bTerrainTooltip;
	float m_td4Tooltip;
	struct CTextToolTip* m_pTextTip;
};


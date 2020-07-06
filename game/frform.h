#pragma once
#include "frwnd.h"

class FrFrame;
class FrStatic;
class FrEdit;

enum eFormRet {
	FrNONE = 0x0,
	FrOK = 0x1,
	FrCANCEL = 0x2,
	FrYES = 0x3,
	FrNO = 0x4,
};

class FrForm : public FrWnd {
public:

private:
	bool m_canDrag;
	bool m_bCaptionDown;
	bool m_bResizeBtnDown;
	unsigned int m_flag;
	eFormRet m_retCode;
	bool (FrCmdTarget::* m_pFnResult)(int, FrForm*);
	FrCmdTarget* m_pCmdDest;
	FrFrame* m_pBaseFrm;
	FrStatic* m_pMsgStatic;
	FrEdit* m_pMsgEdit;
	float m_timeLimit;
	float m_dt;
	eFormRet m_defaultRetCode;
};

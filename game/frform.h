#pragma once
#include "frwnd.h"

class FrFrame;
class FrStatic;
class FrEdit;

enum eFormRet {
	FrNONE,
	FrOK,
	FrCANCEL,
	FrYES,
	FrNO,
};

class FrForm : public FrWnd {
public:

private:
	static bool m_bHasTail;
	
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

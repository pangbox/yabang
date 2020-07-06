#pragma once

class FrCmdTarget;

enum eFrCmd {
	FRCMD_NONE,
	FRCMD_INIT,
	FRCMD_MOUSEMOVE,
	FRCMD_LBUTTONDOWN,
	FRCMD_LBUTTONUP,
	FRCMD_RBUTTONDOWN,
	FRCMD_RBUTTONUP,
	FRCMD_DBLCLICK,
	FRCMD_OWNERDRAW,
	FRCMD_ENTERKEY,
	FRCMD_LOSTKEYFOCUS,
	FRCMD_HOVERON,
	FRCMD_HOVEROFF,
	FRCMD_FINISH,
	FRCMD_DESTROY,
};

enum eFrFuncType {
	FrFuncNone,
	FrFuncVV,
	FrFuncBV,
	FrFuncBI,
	FrFuncVI,
	FrFuncBI2,
	FrFuncBI3,
};

typedef void (FrCmdTarget::* ProcFuncVoidVoid)();
typedef bool (FrCmdTarget::* ProcFuncBoolVoid)();
typedef bool (FrCmdTarget::* ProcFuncBoolInt)(int);
typedef void (FrCmdTarget::* ProcFuncVoidInt)(int);

union FreshHandlerProc {
	ProcFuncVoidVoid vv;
	ProcFuncBoolVoid bv;
	ProcFuncBoolInt bi;
	ProcFuncVoidInt vi;
};

struct sFRESH_ENTRY {
	char* pName;
	eFrCmd cmd;
	eFrFuncType eFt;
	FreshHandlerProc pFn;
};

struct sFRESH_HANDLER {
	FrCmdTarget* pFresh;
	eFrFuncType eFt;
	FreshHandlerProc pFn;
};

struct sFRESH_MSGMAP {
	sFRESH_MSGMAP* pBaseMsgMap;
	sFRESH_ENTRY* pEntry;
};

class FrCmdTarget {
public:
	FrCmdTarget();
	virtual ~FrCmdTarget();

	FrCmdTarget(const FrCmdTarget&) = default;
	FrCmdTarget &operator =(const FrCmdTarget&) = default;

	FrCmdTarget(FrCmdTarget&&) = default;
	FrCmdTarget& operator =(FrCmdTarget&&) = default;

	virtual const sFRESH_MSGMAP* GetMessageMap();
	sFRESH_ENTRY* FindFreshEntry(sFRESH_ENTRY* pBegin, const char* pName, eFrCmd cmd);
	bool OnFreshMsg(const char* pName, eFrCmd cmd, int var1, sFRESH_HANDLER* pHandler);
};

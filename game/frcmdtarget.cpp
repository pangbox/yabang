#include "frcmdtarget.h"

#include <cstring>

// TODO: Down the road, this should be re-architected to use safer typing.
bool __fastcall FreshDispatchMsg(FrCmdTarget* pTarget, int var1, sFRESH_ENTRY* pEntry, sFRESH_HANDLER* pHandler) {
    if (pHandler) {
        pHandler->pFn = pEntry->pFn;
        pHandler->eFt = pEntry->eFt;
        pHandler->pFresh = pTarget;
        return true;
    } else {
        switch (pEntry->eFt)
        {
        case FrFuncVV:
            (pTarget->*pEntry->pFn.vv)();
            return true;
        case FrFuncBV:
            return (pTarget->*pEntry->pFn.bv)();
        case FrFuncBI:
            return (pTarget->*pEntry->pFn.bi)(var1);
        case FrFuncVI:
            (pTarget->*pEntry->pFn.vi)(var1);
            return true;
        default:
            return true;
        }
    }
}

FrCmdTarget::FrCmdTarget() = default;

FrCmdTarget::~FrCmdTarget() = default;

const sFRESH_MSGMAP* FrCmdTarget::GetMessageMap() {
	return nullptr;
}

sFRESH_ENTRY* FrCmdTarget::FindFreshEntry(sFRESH_ENTRY* pBegin, const char* pName, eFrCmd cmd) {
    for (auto *i = pBegin; i; ++i) {
        if (!_stricmp(i->pName, pName) && i->cmd == cmd) {
            return i;
        }
    }
    for (auto* i = pBegin; i; ++i) {
        if (!*i->pName && i->cmd == cmd) {
            return i;
        }
    }
    return nullptr;
}

bool FrCmdTarget::OnFreshMsg(const char* pName, eFrCmd cmd, int var1, sFRESH_HANDLER* pHandler) {
    for (const auto *i = this->GetMessageMap(); i; i = i->pBaseMsgMap) {
        auto *entry = i->pEntry;
        if (!entry) {
            break;
        }
        entry = FindFreshEntry(entry, pName, cmd);
        if (entry) {
            return FreshDispatchMsg(this, var1, entry, pHandler);
        }
    }
    return true;
}

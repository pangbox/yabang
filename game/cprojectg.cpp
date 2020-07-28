#include "cprojectg.h"

void CProjectG::FpuCheck() {
    this->m_fpControl = _controlfp(0, 0);
    if (this->m_fpControl != (PC_24 | MCW_EM)) {
        this->m_fpControl = _controlfp(PC_24, MCW_PC);
    }
}

void CProjectG::SetMoveLoginServer(const char* unk1, const char* unk2) {
    this->m_bUnkFlag2 = true;
    this->m_szUnkLogin1 = unk1;
    this->m_szUnkLogin2 = unk2;
}
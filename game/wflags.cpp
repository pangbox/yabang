#include "wflags.h"

WFlags::WFlags() = default;

WFlags::WFlags(uint32_t& flag)
	: m_flag(flag) {}

void WFlags::Set(uint32_t flag) {
	this->m_flag = flag;
}

void WFlags::Reset() {
	this->m_flag = 0;
}

void WFlags::Disable(uint32_t flag) {
	this->m_flag &= ~flag;
}

void WFlags::Turn(uint32_t flag, bool on) {
	if (on) {
		this->m_flag |= flag;
	} else {
		this->m_flag &= ~flag;
	}
}

bool WFlags::GetFlag(uint32_t flag) {
	return (flag & this->m_flag) != 0;
}

void WFlags::Enable(uint32_t flag) {
	this->m_flag |= flag;
}

WFlags::operator unsigned() const {
	return m_flag;
}

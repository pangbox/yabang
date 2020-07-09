#pragma once
#include <cstdint>

class WFlags {
public:
	WFlags();
	explicit WFlags(uint32_t& flag);

	void Set(uint32_t flag);
	void Reset();
	void Disable(uint32_t flag);
	void Turn(uint32_t flag, bool on);
	[[nodiscard]] bool GetFlag(uint32_t flag) const;
	void Enable(uint32_t flag);

	operator uint32_t() const;

private:
	uint32_t m_flag = 0;
};

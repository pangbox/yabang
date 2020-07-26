#pragma once
#include <cstdint>

#include "wmath.h"

extern WMatrix g_camera;

class CMainFrame {
public:
	CMainFrame();
	virtual ~CMainFrame();

	virtual void Process(float time) = 0;
	virtual void Draw() = 0;
	virtual void Paint();
	[[nodiscard]] virtual uint32_t GetSystemTime() const = 0;
	virtual void Update(int);

	void SetGameSpeed(float gameSpeed);
	[[nodiscard]] float GetGameSpeed() const;
	void ResetTimer();
	void SetFPS(float fps);
	void Quit();
	void Restart();
	[[nodiscard]] float GetFPS() const;
	uint32_t GetTime() const;
	uint32_t Main(int flags);

protected:
	uint32_t m_dwStatus;
	float m_fFPS;
	float m_gameSpeed;
	float m_fNextFrame;
	uint32_t m_dwSystemTime;
	uint32_t m_dwFrameSkip;
};

#include "cmainframe.h"

WMatrix g_camera;

CMainFrame::CMainFrame() {
	this->m_dwStatus = 0;
	this->m_fNextFrame = 0.0;
	this->m_gameSpeed = 1.0;
	this->m_fFPS = 30.0;
	this->m_dwFrameSkip = 1;

	g_camera.xa.x = 1.0f;
	g_camera.xa.y = 0.0f;
	g_camera.xa.z = 0.0f;
	g_camera.ya.x = 0.0f;
	g_camera.ya.y = 1.0f;
	g_camera.ya.z = 0.0f;
	g_camera.za.x = 0.0f;
	g_camera.za.y = 0.0f;
	g_camera.za.z = 1.0f;
	g_camera.pivot.y = 0.0f;
	g_camera.pivot.z = 0.0f;
	g_camera.pivot.x = 0.0f;
}

CMainFrame::~CMainFrame() = default;

void CMainFrame::SetGameSpeed(float gameSpeed) {
	this->m_gameSpeed = gameSpeed;
}

float CMainFrame::GetGameSpeed() const {
	return this->m_gameSpeed;
}

void CMainFrame::ResetTimer() {
	this->m_dwSystemTime = this->GetSystemTime();
	this->m_fNextFrame = 0.0f;
}

void CMainFrame::SetFPS(float fps) {
	this->m_fFPS = fps;
	this->m_dwFrameSkip = 1;
}

void CMainFrame::Quit() {
	this->m_dwStatus = 1;
}

void CMainFrame::Restart() {
	this->m_dwStatus = 2;
}

float CMainFrame::GetFPS() const {
	return this->m_fFPS / static_cast<float>(this->m_dwFrameSkip);
}

void CMainFrame::Paint() {}

void CMainFrame::Update(int) {}

uint32_t CMainFrame::GetTime() const {
	return this->GetSystemTime() - this->m_dwSystemTime;
}

uint32_t CMainFrame::Main(int flags) {
	float currentTime = static_cast<float>(this->GetTime());
	if ((flags & 4) == 0 && this->m_fNextFrame + 1500.0f < currentTime) {
		this->m_fNextFrame = currentTime - 100.0f;
	}
	while (currentTime > this->m_fNextFrame) {
		float frameTime = currentTime - this->m_fNextFrame;
		if (frameTime > 100.0) {
			frameTime = 100.0;
		}
		this->m_fNextFrame += frameTime;
		int updateTime = (flags & 0x10) != 0 ? 0 : static_cast<int>(this->m_fNextFrame);
		this->Update(updateTime);
		float dt = frameTime * 0.001f;
		this->Process(dt);
	}
	if (!this->m_dwStatus && (flags & 1) == 0) {
		this->Draw();
		this->Paint();
	}
	return this->m_dwStatus;
}

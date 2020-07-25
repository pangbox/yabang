#pragma once
#include "frgraphicinterface.h"
#include "wlist.h"
#include "wmath.h"

class WMilesSoundSystem;
class CSoundObject;
class CSoundManager;

struct _SFX_INFO {
	char name[64];
	char alias[32];
	uint32_t streamhandle;
	uint32_t dwStartTick;
	WVector pos;
	bool bIs3d;
	bool bPlayingNearby;
	bool bPlaying;
	uint32_t flags;
	const CSoundObject* soundObj;
};

struct CSoundObject {
	WVector pos;
};

class CSoundManager {
public:
	CSoundManager();
	~CSoundManager();

	void SetFadeOutTime(float value);
	void Init();
	void Destroy();
	void SetMSS(WMilesSoundSystem* mss);
	WMilesSoundSystem* GetMSS() const;
	void Activate(bool active);
	int GetSoundFx(const char* filename, bool use3d) const;
	bool IsPlayingBGM() const;
	float GetVolume() const;
	void StopBGM(bool save);
	void PauseBGM(bool save);
	void SetBGMVolume(float volume);
	void SetBGMVolumeInBGMArea(float volume) const;
	float GetBGMVolume() const;
	void DestroyBGM();
	void SetSpeakerType(int speakerType) const;
	void PlaySfx(const char* filename, const WVector* pos, unsigned flags, const char* alias,
	             const CSoundObject* soundObj, float minVol, float maxVol);
	void PlayBGM(const char* bgmName, bool once, int save);
	_SFX_INFO* FindIdleSfx(const char* name);
	_SFX_INFO* LoadSfx(const char* name, const WVector* pos, unsigned flags, const char* alias,
	                   const CSoundObject* soundObj, float minVol, float maxVol);
	_SFX_INFO* FindSfx(const char* name);
	_SFX_INFO* FindAliasSfx(const char* alias);
	void DelSfx(const char* alias);
	void AllDelSfx();
	void SetVolume(float volume);
	void SetSFXVolumeWhilePlaying(const char* alias, float volume);
	void StopAllSound();
	void CheckIdleSfx(float dt);
	void StopSfx(const char* alias);
	bool IsPlaying(const char* name);
	void AddSfx(_SFX_INFO* sfx);
	void Process(float dt, WView* view);
	void DelSfx(_SFX_INFO* sfx);

protected:
	WMilesSoundSystem* m_mss{};
	float m_volume{};
	uint32_t m_bgmStreamHandle{};
	uint32_t m_bgmStreamHandle2{};
	float m_bgmVolume2{};
	float m_bgmFadePos{};
	float m_fadeOutTime{};
	float m_bgmVolume{};
	char m_bgmName[64]{};
	float m_fDistNearY{};
	float m_fDistFarY{};
	float m_fMaxDistXZ{};
	float m_fTimeCount{};
	bool m_bActive{};
	WList<_SFX_INFO*> m_sfxInfo{16, 16};
	WList<_SFX_INFO*> m_sfxIdleList{16, 16};
};

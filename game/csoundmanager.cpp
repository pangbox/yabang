#include "csoundmanager.h"

#include "mss.h"
#include "wmilessoundsystem.h"
#include "wview.h"

CSoundManager::CSoundManager() = default;

CSoundManager::~CSoundManager() {
	m_mss = nullptr;
}

void CSoundManager::SetFadeOutTime(float value) {
	this->m_fadeOutTime = value;
}

void CSoundManager::Init() {
	this->m_volume = 1.0;
	this->m_bgmVolume = 1.0;
	this->m_bgmVolume2 = 1.0;
	this->m_fadeOutTime = 1.0;
	this->m_mss = nullptr;
	this->m_bActive = true;
	this->m_fTimeCount = 0.0;
	this->m_bgmStreamHandle = 0;
	this->m_bgmStreamHandle2 = 0;
	this->m_bgmFadePos = 0.0;
	this->m_fDistFarY = 160.0;
	this->m_fDistNearY = 128.0;
	this->m_fMaxDistXZ = 960.0;
	memset(this->m_bgmName, 0, sizeof(this->m_bgmName));
}

void CSoundManager::Destroy() {
	this->AllDelSfx();
	this->m_mss = nullptr;
}

void CSoundManager::SetMSS(WMilesSoundSystem* mss) {
	this->m_mss = mss;
}

WMilesSoundSystem* CSoundManager::GetMSS() const {
	return this->m_mss;
}

void CSoundManager::Activate(bool active) {
	this->m_bActive = active;
	if (this->m_mss) {
		this->m_mss->Activate(active);
	}
}

int CSoundManager::GetSoundFx(const char* filename, bool use3d) const {
	if (!this->m_mss || !filename) {
		return 0;
	}
	if (use3d) {
		return this->m_mss->Load(filename, 1);
	}
	if (strstr(filename, ".a") || strstr(filename, ".mp3")) {
		return this->m_mss->Load(filename, 2);
	}
	return this->m_mss->Load(filename, 0);
}

bool CSoundManager::IsPlayingBGM() const {
	return this->m_bgmStreamHandle != 0;
}

float CSoundManager::GetVolume() const {
	return this->m_volume;
}

void CSoundManager::StopBGM(bool save) {
	uint32_t bgmStreamHandle = this->m_bgmStreamHandle;
	if (!bgmStreamHandle || !this->m_mss) {
		return;
	}
	if (save) {
		if (this->m_bgmStreamHandle2)
			this->m_mss->Stop(this->m_bgmStreamHandle2);
		bgmStreamHandle = this->m_bgmStreamHandle;
		float bgmVolume = this->m_bgmVolume;
		this->m_bgmFadePos = 0.0;
		this->m_bgmStreamHandle = 0;
		this->m_bgmStreamHandle2 = bgmStreamHandle;
		this->m_bgmVolume2 = bgmVolume;
	} else {
		this->m_mss->Stop(bgmStreamHandle);
		this->m_bgmStreamHandle2 = 0;
		this->m_bgmStreamHandle = 0;
	}
}

void CSoundManager::PauseBGM(bool save) {
	if (!this->m_bgmStreamHandle || !this->m_mss) {
		return;
	}
	if (save) {
		if (this->m_bgmStreamHandle2) {
			this->m_mss->Stop(this->m_bgmStreamHandle2);
		}
		this->m_bgmStreamHandle2 = this->m_bgmStreamHandle;
		this->m_bgmFadePos = 0.0;
		this->m_bgmVolume2 = this->m_bgmVolume;
	} else {
		this->m_mss->Stop(this->m_bgmStreamHandle);
	}
}

void CSoundManager::SetBGMVolume(float volume) {
	this->m_bgmVolume = volume;
	if (!this->m_bgmStreamHandle || !this->m_mss) {
		return;
	}
	this->m_mss->SetVolume(this->m_bgmStreamHandle, volume);
}

void CSoundManager::SetBGMVolumeInBGMArea(float volume) const {
	if (!this->m_bgmStreamHandle || !this->m_mss) {
		return;
	}
	this->m_mss->SetVolume(this->m_bgmStreamHandle, volume * this->m_bgmVolume);
}

float CSoundManager::GetBGMVolume() const {
	return this->m_bgmVolume;
}

void CSoundManager::DestroyBGM() {
	if (!this->m_bgmStreamHandle) {
		return;
	}
	if (this->m_mss) {
		this->m_mss->DestroySoundBuffer(this->m_bgmStreamHandle);
	}
	this->m_bgmStreamHandle = 0;
}

void CSoundManager::SetSpeakerType(int speakerType) const {
	if (!this->m_mss) {
		return;
	}
	this->m_mss->SpeakerType(static_cast<MSS_MC_SPEC>(speakerType));
}

void CSoundManager::PlaySfx(const char* filename, const WVector* pos, unsigned int flags, const char* alias,
                            const CSoundObject* soundObj, float minVol, float maxVol) {
	_SFX_INFO* sfx = this->LoadSfx(filename, pos, flags, alias, soundObj, minVol, maxVol);
	if (!sfx || !sfx->bPlayingNearby) {
		return;
	}
	this->m_mss->Play(sfx->streamhandle, sfx->flags & 1, true);
}

void CSoundManager::PlayBGM(const char* bgmName, bool once, int save) {
	if (!this->m_mss) {
		return;
	}

	if (bgmName) {
		if (this->m_bgmStreamHandle && !_stricmp(bgmName, this->m_bgmName)) {
			if (!this->m_mss->IsPlaying(this->m_bgmStreamHandle)) {
				this->m_mss->Resume(this->m_bgmStreamHandle, !once);
			}
		} else {
			DWORD bgmStreamHandle = this->GetSoundFx(bgmName, false);
			if (bgmStreamHandle) {
				this->StopBGM(save);
				this->m_bgmStreamHandle = bgmStreamHandle;
				strcpy(this->m_bgmName, bgmName);
				this->m_mss->Play(this->m_bgmStreamHandle, !once, true);
				this->SetBGMVolume(this->m_bgmVolume);
			}
		}
	} else {
		this->StopBGM(save);
		this->m_bgmName[0] = 0;
	}
}

_SFX_INFO* CSoundManager::FindIdleSfx(const char* name) {
	if (!this->m_mss) {
		return nullptr;
	}
	for (auto* it = this->m_sfxInfo.Start(); it != nullptr; it = this->m_sfxInfo.Next()) {
		if (strcmpi(it->name, name) != 0
			|| it->bPlaying && !it->bPlayingNearby
			|| this->m_mss->IsPlaying(it->streamhandle)) {
			continue;
		}
		return it;
	}
	return nullptr;
}

_SFX_INFO* CSoundManager::LoadSfx(const char* name, const WVector* pos, unsigned int flags, const char* alias,
                                  const CSoundObject* soundObj, float minVol, float maxVol) {
	if (!this->m_mss) {
		return nullptr;
	}
	bool is3d = pos && flags != 5;
	_SFX_INFO* sfx = this->FindIdleSfx(name);
	if (!sfx) {
		char filename[260];
		sprintf(filename, "sound/%s.wav", name);
		int sfxHandle = this->GetSoundFx(filename, is3d);
		if (!sfxHandle)
			return nullptr;
		sfx = new _SFX_INFO;
		if (!sfx) {
			this->m_mss->DestroySoundBuffer(sfxHandle);
			return nullptr;
		}
		memset(sfx, 0, sizeof(_SFX_INFO));
		strcpy(sfx->name, name);
		sfx->streamhandle = sfxHandle;
		this->AddSfx(sfx);
	}
	float volume = this->m_volume;
	memset(sfx->alias, 0, sizeof sfx->alias);
	strcpy(sfx->alias, alias);
	sfx->flags = flags;
	sfx->bPlaying = true;
	sfx->bIs3d = is3d;
	sfx->bPlayingNearby = !is3d;
	sfx->dwStartTick = GetTickCount();
	if (!pos || flags == 5) {
		if (pos && flags == 5) {
			float distance = sqrt(pos->z * pos->z + pos->y * pos->y + pos->x * pos->x);
			float posVol = (maxVol - distance) / maxVol >= minVol ? (maxVol - distance) / maxVol : minVol;
			sfx->flags = 0;
			volume = posVol * volume;
		}
	} else {
		sfx->pos = *pos;
	}
	sfx->soundObj = (sfx->flags & 4) != 0 ? soundObj : nullptr;
	this->m_mss->SetVolume(sfx->streamhandle, volume);
	return sfx;
}

_SFX_INFO* CSoundManager::FindSfx(const char* name) {
	for (auto* it = this->m_sfxInfo.Start(); it != nullptr; it = this->m_sfxInfo.Next()) {
		if (strcmpi(it->name, name) == 0) {
			return it;
		}
	}
	return nullptr;
}

_SFX_INFO* CSoundManager::FindAliasSfx(const char* alias) {
	for (auto* it = this->m_sfxInfo.Start(); it != nullptr; it = this->m_sfxInfo.Next()) {
		if (strcmpi(it->alias, alias) == 0) {
			return it;
		}
	}
	return nullptr;
}

void CSoundManager::DelSfx(const char* alias) {
	_SFX_INFO* sfx = this->FindAliasSfx(alias);
	if (sfx) {
		this->DelSfx(sfx);
	}
}

void CSoundManager::AllDelSfx() {
	for (auto* it = this->m_sfxInfo.Start(); it != nullptr; it = this->m_sfxInfo.Next()) {
		this->DelSfx(it);
	}
	if (this->m_bgmStreamHandle && this->m_mss) {
		this->m_mss->DestroySoundBuffer(this->m_bgmStreamHandle);
		this->m_bgmStreamHandle = 0;
	}
}

void CSoundManager::SetVolume(float volume) {
	this->m_volume = volume;
	if (!this->m_mss) {
		return;
	}
	for (auto* it = this->m_sfxInfo.Start(); it != nullptr; it = this->m_sfxInfo.Next()) {
		this->m_mss->SetVolume(it->streamhandle, volume);
	}
}

void CSoundManager::SetSFXVolumeWhilePlaying(const char* alias, float volume) {
	_SFX_INFO* sfx = this->FindAliasSfx(alias);
	if (!sfx || !this->m_mss) {
		return;
	}
	this->m_mss->SetVolume(sfx->streamhandle, volume * this->m_volume);
}

void CSoundManager::StopAllSound() {
	if (!this->m_mss) {
		return;
	}
	for (auto* it = this->m_sfxInfo.Start(); it != nullptr; it = this->m_sfxInfo.Next()) {
		if (!it->bPlaying) {
			continue;
		}
		this->m_mss->Stop(it->streamhandle);
		it->bPlaying = false;
		delete it;
	}
	this->m_sfxInfo.Reset();
}

void CSoundManager::CheckIdleSfx(float dt) {
	if (!this->m_bActive || !this->m_mss) {
		return;
	}

	this->m_fTimeCount += dt;
	if (this->m_fTimeCount <= 5.0) {
		return;
	}
	this->m_fTimeCount = 0.0;

	DWORD now = GetTickCount();
	for (auto* it = this->m_sfxInfo.Start(); it != nullptr; it = this->m_sfxInfo.Next()) {
		if (now - it->dwStartTick <= 50000 || this->m_mss->IsPlaying(it->streamhandle)) {
			continue;
		}
		this->m_sfxIdleList += it;
	}
	for (auto* it = this->m_sfxIdleList.Start(); it != nullptr; it = this->m_sfxIdleList.Next()) {
		this->DelSfx(it);
	}
	this->m_sfxIdleList.Reset();
}

void CSoundManager::StopSfx(const char* alias) {
	if (!this->m_mss) {
		return;
	}
	if (alias) {
		auto* sfx = this->FindAliasSfx(alias);
		if (sfx && sfx->streamhandle && sfx->bPlaying) {
			this->m_mss->Stop(sfx->streamhandle);
			sfx->bPlaying = false;
		}
	} else {
		for (auto* it = this->m_sfxInfo.Start(); it != nullptr; it = this->m_sfxInfo.Next()) {
			if (!it->bPlaying) {
				continue;
			}
			this->m_mss->Stop(it->streamhandle);
			it->bPlaying = false;
		}
	}
}

bool CSoundManager::IsPlaying(const char* name) {
	if (!strlen(name)) {
		return false;
	}
	auto* sfx = this->FindAliasSfx(name);
	if (sfx && sfx->streamhandle && this->m_mss) {
		return this->m_mss->IsPlaying(sfx->streamhandle);
	}
	if (!_stricmp(name, this->m_bgmName)) {
		return this->m_mss->IsPlaying(this->m_bgmStreamHandle);
	}
	return false;
}

void CSoundManager::AddSfx(_SFX_INFO* sfx) {
	this->m_sfxInfo.AddItem(sfx, nullptr, false);
}

void CSoundManager::Process(float dt, WView* view) {
	if (!this->m_mss) {
		return;
	}

	float maxDistSquared = this->m_fMaxDistXZ * this->m_fMaxDistXZ;
	float distFadeRange = this->m_fDistFarY - this->m_fDistNearY;

	if (this->m_bgmStreamHandle2) {
		this->m_bgmFadePos += dt;
		if (this->m_bgmFadePos <= m_fadeOutTime) {
			float fadedVol = (1.0f - this->m_bgmFadePos / this->m_fadeOutTime) * this->m_bgmVolume2;
			this->m_mss->SetVolume(this->m_bgmStreamHandle2, fadedVol);
		} else {
			this->m_mss->Stop(this->m_bgmStreamHandle2);
			this->m_bgmStreamHandle2 = 0;
			this->m_bgmFadePos = 0.0;
		}
	}

	for (auto* sfx = this->m_sfxInfo.Start(); sfx != nullptr; sfx = this->m_sfxInfo.Next()) {
		if (!sfx->bPlaying || !sfx->bIs3d) {
			continue;
		}
		if ((sfx->flags & 4) != 0) {
			sfx->pos = sfx->soundObj->pos;
			WVector transformed = sfx->pos * view->GetInvCamera();
			this->m_mss->SetPosition(sfx->streamhandle, transformed.x, transformed.y, transformed.z);
		}
		if ((sfx->flags & 2) == 0 || !sfx->bPlayingNearby) {
			auto camera = view->GetCamera();
			float dy = camera.pivot.y - sfx->pos.y;
			if (dy <= 0.0) {
				dy = -dy;
			}

			if (dy >= this->m_fDistNearY) {
				float fadedVol = (this->m_fDistFarY - dy) / distFadeRange * this->m_volume;
				if (fadedVol <= 0.0) {
					fadedVol = 0.0;
				}
				this->m_mss->SetVolume(sfx->streamhandle, fadedVol);
			}
			WVector transformed = sfx->pos * view->GetInvCamera();
			this->m_mss->SetPosition(sfx->streamhandle, transformed.x, transformed.y, transformed.z);
			if (!sfx->bPlayingNearby) {
				float dx = camera.pivot.x - sfx->pos.x;
				float dz = camera.pivot.z - sfx->pos.z;
				if (dz * dz + dx * dx < maxDistSquared && dy < this->m_fDistFarY || (sfx->flags & 1) != 0) {
					this->m_mss->Play(sfx->streamhandle, sfx->flags & 1, true);
					sfx->bPlayingNearby = true;
				}
			}
		}
	}

	this->CheckIdleSfx(dt);
}

void CSoundManager::DelSfx(_SFX_INFO* sfx) {
	this->m_sfxInfo.DelItem(sfx);
	if (this->m_mss) {
		this->m_mss->DestroySoundBuffer(sfx->streamhandle);
	}
	delete sfx;
}

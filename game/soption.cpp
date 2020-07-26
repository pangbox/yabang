#include "soption.h"

#include <cstring>

bool sOption::sbHwTnlSupport = false;
bool sOption::sbRtSupport = false;
bool sOption::sbVsSupport = false;
bool sOption::sbPsSupport = false;
bool sOption::sbMrtSupport = false;
bool sOption::sbClipPlaneSupport = false;

sOption::sOption() {
	this->m_vScreenWidth2 = 800;
	this->m_vScreenWidth = 800;
	this->m_vMipMapCreateFilter = 2;
	this->m_aMssChannels = 2;
	this->m_vScreenHeight2 = 600;
	this->m_vScreenHeight = 600;
	this->m_vScreenColor = 16;
	this->m_aMssBits = 16;
	this->m_aSfxVolume = 1.0f;
	this->m_aBgmVolume = 1.0f;
	this->m_gMouseSensitivity = 1.0f;
	this->m_wWindowedMode = 0;
	this->m_vFillMode = 0;
	this->m_vWideMode = 0;
	this->m_vOverall = -1;
	this->m_vGameScreen = 0;
	this->m_vTnLMode = 1;
	this->m_vMipDDSRes = 0;
	this->m_vUseMipmap = 0;
	this->m_vMaxMipLevel = 1;
	this->m_vParticleLevel = 1;
	this->m_vShadowEnabled = 1;
	this->m_vShadowmapEnabled = 1;
	this->m_vReflectionEnabled = 1;
	this->m_vLodEnabled = 0;
	this->m_gNpcEnabled = 1;
	this->m_vScreenRatio = 1.3f;
	this->m_aOverall = 0;
	this->m_aMssEnabled = 1;
	this->m_aMssFrequency = 44100;
	this->m_aMssHwSoundEnabled = 1;
	this->m_aMssBalance = 128;
	this->m_aMssSpeaker = 0;
	this->m_gWhisper = 1;
	this->m_gInvitation = 1;
	this->m_gFriendConfirm = 1;
	this->m_gTransChatWin = 1;
	this->m_gUiEffect = 1;
	this->m_gLastLoginId[0] = 0;
	this->m_gIdentity = 0;
	this->m_gExtendChatLine = 0;
	this->m_gUserSort = 0;
	this->m_bPPLEnabled = 1;
	this->m_gPPLSize = 0;
	this->m_gPowerGauge = 1;
	this->m_gCaptureLogo = 1;
	this->m_gRestore = 0;
	this->m_mChatWndUp = 0;
	this->m_gUnderExtBtn = 0;
	this->m_gAvatarNewBie = 1;
	this->m_mCaptureHideGUI = 0;
	this->m_mCaptureHidePII = 0;
	this->m_gTerrainTooltip = 1;
	this->m_gCutinFlag = 1;
	for (auto* i : this->m_mMacroText) {
		i[0] = 0;
	}
	this->m_mSaveMacros = 1;
}

sOption::~sOption() = default;

void sOption::vSetOverallByVidOptions() {
	if (this->m_vScreenWidth == 1024
		&& this->m_vScreenHeight == 768
		&& this->m_vScreenColor == 32
		&& !this->m_vMipDDSRes
		&& !this->m_vParticleLevel
		&& this->m_vShadowEnabled == 1
		&& this->m_vShadowmapEnabled == 1
		&& this->m_vReflectionEnabled == 1
		&& !this->m_vLodEnabled
		&& this->m_gNpcEnabled == 1) {
		this->m_vOverall = 0;
	} else if (this->m_vScreenWidth == 800
		&& this->m_vScreenHeight == 600
		&& this->m_vScreenColor == 32
		&& this->m_vMipDDSRes == 1
		&& this->m_vParticleLevel == 1
		&& this->m_vShadowEnabled == 1
		&& this->m_vShadowmapEnabled == 1
		&& this->m_vReflectionEnabled == 1
		&& this->m_vLodEnabled == 1
		&& this->m_gNpcEnabled == 1) {
		this->m_vOverall = 1;
	} else if (this->m_vScreenWidth == 640
		&& this->m_vScreenHeight == 480
		&& this->m_vScreenColor == 16
		&& this->m_vMipDDSRes == 2
		&& this->m_vParticleLevel == 2
		&& !this->m_vShadowEnabled
		&& !this->m_vShadowmapEnabled
		&& !this->m_vReflectionEnabled
		&& this->m_vLodEnabled == 1
		&& !this->m_gNpcEnabled) {
		this->m_vOverall = 2;
	} else {
		this->m_vOverall = -1;
	}
}

bool sOption::IsSupportShadow() {
	return sbRtSupport;
}

bool sOption::IsSupportReflection() {
	return sbRtSupport && sbClipPlaneSupport && sbVsSupport && sbPsSupport;
}

bool sOption::IsSupportLod() {
	return sbRtSupport;
}

const char* sOption::vGetTnlText(int tnl) const {
	switch (tnl) {
		case 0:
			return "소프트웨어 TnL";
		case 1:
			return "하드웨어 TnL";
		default:
			return "";
	}
}

int sOption::vGetTnlLvl(const char* tnl) const {
	if (strcmp(tnl, "소프트웨어 TnL") == 0) {
		return 0;
	}
	if (strcmp(tnl, "하드웨어 TnL") == 0) {
		return 1;
	}
	return -1;
}

const char* sOption::aGetSpeakerText(int speaker) const {
	switch (speaker) {
		case 0:
			return "2채널 스피커";
		case 1:
			return "헤드폰";
		case 2:
			return "서라운드";
		case 3:
			return "4채널 스피커";
		case 4:
			return "5.1채널 스피커";
		case 5:
			return "7.1채널 스피커";
		default:
			return "";
	}
}

int sOption::aGetSpeakerLvl(const char* speaker) const {
	for (int i = 0; i < 6; i++) {
		if (strcmp(speaker, this->aGetSpeakerText(i)) == 0) {
			return i;
		}
	}
	return 0;
}

const char* sOption::GetThreeLvlText(int level) const {
	switch (level) {
		case 0:
			return "높음";
		case 1:
			return "중간";
		case 2:
			return "낮음";
		default:
			return "사용자";
	}
}

const char* sOption::GetTwoLvlText(int level) const {
	switch (level) {
		case 0:
			return "높음";
		case 1:
			return "낮음";
		default:
			return "사용자";
	}
}

const char* sOption::vGetOverallText(int level) const {
	return this->GetThreeLvlText(level);
}

const char* sOption::vGetTexResText(int level) const {
	return this->GetThreeLvlText(level);
}

const char* sOption::vGetParticleText(int level) const {
	return this->GetThreeLvlText(level);
}

int sOption::vGetLvl3FromText(const char* level) const {
	for (int i = 0; i < 3; i++) {
		if (strcmp(level, this->GetThreeLvlText(i)) == 0) {
			return i;
		}
	}
	return -1;
}

void sOption::vResetByOverall(const char* overall) {
	if (!strcmp(overall, "높음")) {
		this->m_vOverall = 0;
		this->m_vGameScreen = 0;
		this->m_vScreenWidth = 1024;
		this->m_vScreenHeight = 768;
		this->m_vScreenColor = 32;
		this->m_vMipDDSRes = 0;
		this->m_vParticleLevel = 0;
		this->m_vShadowmapEnabled = 1;
		this->m_vShadowEnabled = this->IsSupportShadow();
		this->m_gNpcEnabled = 1;
		this->m_vLodEnabled = 0;
		this->m_vReflectionEnabled = this->IsSupportReflection();
	} else if (!strcmp(overall, "중간")) {
		this->m_vOverall = 1;
		this->m_vGameScreen = 0;
		this->m_vScreenWidth = 800;
		this->m_vScreenHeight = 600;
		this->m_vScreenColor = 32;
		this->m_vMipDDSRes = 1;
		this->m_vParticleLevel = 1;
		this->m_vShadowmapEnabled = 1;
		this->m_vShadowEnabled = this->IsSupportShadow();
		this->m_vReflectionEnabled = this->IsSupportReflection();
		this->m_gNpcEnabled = 1;
		this->m_vLodEnabled = this->IsSupportLod();
	} else if (!strcmp(overall, "낮음")) {
		this->m_vOverall = 2;
		this->m_vMipDDSRes = 2;
		this->m_vParticleLevel = 2;
		this->m_vGameScreen = 0;
		this->m_vScreenWidth = 640;
		this->m_vScreenHeight = 480;
		this->m_vScreenColor = 16;
		this->m_vShadowEnabled = 0;
		this->m_vShadowmapEnabled = 0;
		this->m_vReflectionEnabled = 0;
		this->m_gNpcEnabled = 0;
		this->m_vLodEnabled = this->IsSupportLod();
	}
}

int sOption::vGetOverallLvl(const char* level) const {
	return this->vGetLvl3FromText(level);
}

int sOption::vGetTexResLvl(const char* level) const {
	return this->vGetLvl3FromText(level);
}

int sOption::vGetParticleLvl(const char* level) const {
	return this->vGetLvl3FromText(level);
}

int sOption::aGetOverallLvl(const char* level) const {
	return this->vGetLvl3FromText(level);
}

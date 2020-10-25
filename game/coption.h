#pragma once
#include <list>
#include <windows.h>
#include "singleton.h"
#include "soption.h"
#include "wregistry.h"

class WDeviceManager;
class WResourceManager;
class WVideoDev;
class WView;
class CSoundManager;
class CInputManager;

class COption : public WSingleton<COption> {
public:
	enum eRegKey {
		RegKeyVer,
		RegKeyWindowMode,
		RegKeyFillMode,
		RegKeyWideMode,
		RegKeyVideoOverall,
		RegKeyScreenWidth,
		RegKeyScreenHeight,
		RegKeyScreenColor,
		RegKeyTnLMode,
		RegKeyMipDDSRes,
		RegKeyMipUseMipmap,
		RegKeyMipMaxMipLvl,
		RegKeyMipMipCreateFilter,
		RegKeyEffectLevel,
		RegKeyProjectionShadow,
		RegKeyShadowmap,
		RegKeyReflection,
		RegKeyLod,
		RegKeyNpc,
		RegKeyCoordX,
		RegKeyCoordY,
		RegKeyGamma,
		RegKeyMssOn,
		RegKeyAudioOverall,
		RegKeyMssFreq,
		RegKeyMssBits,
		RegKeyMssChannels,
		RegKeyMssEnableHwSound,
		RegKeyMssBalance,
		RegKeyMssSpeaker,
		RegKeySfxVolume,
		RegKeyBGMVolume,
		RegKeyMouseSensitivity,
		RegKeyWhisper,
		RegKeyInvitation,
		RegKeyFriendConfirm,
		RegKeyTransChatWin,
		RegKeyUiEffect,
		RegKeyLastLoginID,
		RegKeyIdentity,
		RegKeyExtendChatLine,
		RegKeyUserSort,
		RegKeyPPL_Enable,
		RegKeyPPL_Size,
		RegKeyPowerGauge,
		RegKeyCaptureLogo,
		RegKeyRestore,
		RegKeyChatWnd,
		RegKeyUnderTabBtn,
		RegKeyAvatarNewbie,
		RegKeyCaptureHideGUI,
		RegKeyCaptureHidePI,
		RegKeyTerrainTooltip,
		RegKeyCutinDisplay,
		RegKeyMacro1,
		RegKeyMacro2,
		RegKeyMacro3,
		RegKeyMacro4,
		RegKeyMacro5,
		RegKeyMacro6,
		RegKeyMacro7,
		RegKeyMacro8,
		RegKeySaveMacros,
		RegKeyCount,
	};
	
	struct sDisplayMode {
		uint32_t dwUnk1;
		uint32_t dwUnk2;
		uint32_t dwUnk3;
		uint32_t dwUnk4;
		uint32_t dwUnk5;
		char name[1];
	};

	struct sRegKey {
		const char* path;
		const char* defaultValue;
	};

	COption(HWND hWnd, WDeviceManager* deviceManager, WResourceManager* resourceManager);
	~COption();
	void InitVars();
	bool ReadReg(int index, char* out, int len);
	bool ScanReg_I(int index, int* out);
	bool ScanReg_F(int index, float* out);
	bool ScanReg_S(int a2, char* a3);
	void WriteReg(int index, const char* str);
	bool WriteReg_I(int index, int value);
	bool WriteReg_F(int index, float value);
	bool WriteReg_S(int index, const char* value);
	void SetGlobalView(WView* globalView);
	void SetAudioManager(CSoundManager* audioManager);
	void vApplyMipmap();
	void vSetMinimumScreenSize(int gScreen, int width, int height);
	void vEnableShadow(bool enabled);
	void vEnableShadowmap(bool enabled);
	bool vIsWindowed() const;
	bool vIsWideMode() const;
	bool vIsShadowEnabled() const;
	bool vIsShadowmapEnabled() const;
	bool vIsReflectionEnabled() const;
	bool vIsLodEnabled() const;
	bool vIsNpcEnabled();
	bool aIsMssEnabled();
	bool aIsMssHwSoundEnabled() const;
	uint32_t aGetMssFrequency() const;
	uint32_t aGetMssBits() const;
	uint32_t aGetMssChannels() const;
	uint32_t aGetMssBalance() const;
	uint32_t aGetMssSpeaker() const;
	float aGetSfxVolume() const;
	float aGetBgmVolume() const;
	void aApplyAudioSetting();
	bool gIsWhisperAllowed() const;
	bool gIsInvitationAllowed() const;
	bool gIsFriendConfirmNeeded() const;
	bool gIsChatWinTransparent() const;
	bool gIsChatLineEntended() const;
	bool gIsPPLEnable() const;
	void gSetPPLEnable(bool enable);
	void gSetPPLSize(int value);
	void gResetRestore();
	uint32_t vGetTnLMode();
	void vApplyLobbyScreenSize();
	bool vChangeScreenSize(int iWidth, int iHeight, int iColor);

protected:
	static sRegKey ms_regKeys[63];
	static const char* ms_szCaptureFmt[4];

	WRegistry m_registry;
	HWND m_hWnd;
	WDeviceManager* m_deviceManager;
	WResourceManager* m_resourceManager;
	WVideoDev* m_videoDev{};
	WView* m_globalView{};
	CSoundManager* m_soundManager{};
	CInputManager* m_inputManager{};
	sOption m_sOption1;
	uint32_t m_vTnLMode{};
	bool m_vUnkFlag2{};
	sOption m_sOption2;
	bool m_vUnkFlag3{};
	bool m_swapMouseButtons{};
	bool m_quitWindowed{};
	std::list<sDisplayMode> m_displayModes;
	uint32_t m_dCaptureFmt{};
	uint32_t m_dAviWidth{};
	uint32_t m_dAviHeight{};
	uint32_t m_dAviFps{};
	uint32_t m_dStartMode{};
	uint32_t m_dLobbyCourse{};
	uint32_t m_dDefaultCharacter{};
	uint32_t m_dDefaultCaddie{};
	uint32_t m_dDefaultClub{};
	uint32_t m_dDefaultBall{};
	uint32_t m_dMap{};
	uint32_t m_dStartHole{};
	uint32_t m_dReplayMode{};
	uint32_t m_dNumPlayers{};
	uint32_t m_dwUnk4{};
	uint32_t m_dWeather{};
	uint32_t m_dChatGender{};
	uint32_t m_dVtxClearTestEnabled{};
	uint32_t m_dBaseMeshTestEnabled{};
	uint32_t m_dClickSystem{};
	uint32_t m_dGameType{};
	uint32_t m_dIsVersionInfoEnabled{};
	float m_gBarY{};
};


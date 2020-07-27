#include "coption.h"

#include "wvideo.h"

COption::sRegKey COption::ms_regKeys[RegKeyCount] = {
	{R"(Ver)", "1.xx"},
	{R"(Option\vWindowMode)", "0"},
	{R"(Option\vFillMode)", "0"},
	{R"(Option\vWideMode)", "0"},
	{R"(Option\vOverall)", "-1"},
	{R"(Option\vScreenWidth)", "800"},
	{R"(Option\vScreenHeight)", "600"},
	{R"(Option\vScreenColor)", "32"},
	{R"(Option\vTnLMode)", "1"},
	{R"(Option\Mip\vDDSRes)", "0"},
	{R"(Option\Mip\vUseMipmap)", "0"},
	{R"(Option\Mip\vMaxMipLvl)", "1"},
	{R"(Option\Mip\vMipCreateFilter)", "2"},
	{R"(Option\vEffectLevel)", "1"},
	{R"(Option\vProjectionShadow)", "1"},
	{R"(Option\vShadowmap)", "1"},
	{R"(Option\vReflection)", "0"},
	{R"(Option\vLod)", "0"},
	{R"(Option\vNpc)", "1"},
	{R"(Option\vCoordX)", "0"},
	{R"(Option\vCoordY)", "0"},
	{R"(Option\vGamma)", "1.3"},
	{R"(Option\aMssOn)", "1"},
	{R"(Option\aOverall)", "0"},
	{R"(Option\aMssFreq)", "44100"},
	{R"(Option\aMssBits)", "16"},
	{R"(Option\aMssChannels)", "2"},
	{R"(Option\aMssEnableHwSound)", "1"},
	{R"(Option\aMssBalance)", "128"},
	{R"(Option\aMssSpeaker)", "0"},
	{R"(Option\aSfxVolume)", "1.00"},
	{R"(Option\aBGMVolume)", "1.00"},
	{R"(Option\gMouseSensitivity)", "1.00"},
	{R"(Option\gWhisper)", "1"},
	{R"(Option\gInvitation)", "1"},
	{R"(Option\gFriendConfirm)", "1"},
	{R"(Option\gTransChatWin)", "1"},
	{R"(Option\gUiEffect)", "1"},
	{R"(Option\gLastLoginID)", ""},
	{R"(Option\gIdentity)", "0"},
	{R"(Option\gExtendChatLine)", "0"},
	{R"(Option\gUserSort)", "0"},
	{R"(Option\gPPL_Enable)", "1"},
	{R"(Option\gPPL_Size)", "0"},
	{R"(Option\gPowerGauge)", "1"},
	{R"(Option\gCaptureLogo)", "1"},
	{R"(Option\gRestore)", "0"},
	{R"(Option\gChatWnd)", "0"},
	{R"(Option\gUnderTabBtn)", "0"},
	{R"(Option\gAvatarNewbie)", "1"},
	{R"(Option\gCaptureHideGUI)", "0"},
	{R"(Option\gCaptureHidePI)", "0"},
	{R"(Option\gTerrainTooltip)", "1"},
	{R"(Option\gCutinDisplay)", "1"},
	{R"(Option\mMacro1)", "만나서 반갑습니다.(인사)"},
	{R"(Option\mMacro2)", "하이룽~ 반가워요~(윙크)"},
	{R"(Option\mMacro3)", "레디해 주세요~(화남)"},
	{R"(Option\mMacro4)", "나이스~ 샷!(박수)"},
	{R"(Option\mMacro5)", "들어가라~ 들어가라~ 들어가라~(긴장)"},
	{R"(Option\mMacro6)", "파이팅~ 힘내세요~(헤헤)"},
	{R"(Option\mMacro7)", "앗! 실수..(엉엉)"},
	{R"(Option\mMacro8)", "수고하셨어요~(인사)"},
	{R"(Option\mSaveMacros)", "1"},
};

const char* COption::ms_szCaptureFmt[4] = {
	"jpg",
	"bmp",
	"avi",
	nullptr,
};

COption::COption(HWND hWnd, WDeviceManager* deviceManager, WResourceManager* resourceManager)
	: m_hWnd(hWnd), m_deviceManager(deviceManager), m_resourceManager(resourceManager) {
	this->InitVars();
	// TODO
	// this->Init();
}

COption::~COption() {
	// TODO
	// this->SaveReg();
}

void COption::InitVars() {
	this->m_videoDev = nullptr;
	this->m_soundManager = nullptr;
	this->m_inputManager = nullptr;
	this->m_vUnkFlag2 = false;
	this->m_gBarY = this->m_sOption1.m_vScreenHeight - 62.0f;
	this->m_swapMouseButtons = false;
	this->m_quitWindowed = false;
	this->m_dCaptureFmt = 0;
	this->m_dDefaultCharacter = 0;
	this->m_dDefaultCaddie = 0;
	this->m_dAviWidth = 320;
	this->m_dAviHeight = 240;
	this->m_dAviFps = 10;
	this->m_dStartMode = 0;
	this->m_dLobbyCourse = -1;
	this->m_dMap = 0;
	this->m_dStartHole = 1;
	this->m_dReplayMode = 0;
	this->m_dNumPlayers = 1;
	this->m_dwUnk4 = 1;
	this->m_dWeather = 0;
	this->m_dClickSystem = 0;
	this->m_dVtxClearTestEnabled = 0;
	this->m_dBaseMeshTestEnabled = 0;
}

bool COption::ReadReg(int index, char* out, int len) {
	if (index < 0 || index >= RegKeyCount) {
		return false;
	}
	return this->m_registry.Read(ms_regKeys[index].path, out, len);
}

bool COption::ScanReg_I(int index, int* out) {
	if (index < 0 || index >= RegKeyCount || !strlen(ms_regKeys[index].path)) {
		return false;
	}
	return this->m_registry.Scan(ms_regKeys[index].path, "%d", out) != 0;
}

bool COption::ScanReg_F(int index, float* out) {
	if (index < 0 || index >= RegKeyCount || !strlen(ms_regKeys[index].path)) {
		return false;
	}
	return this->m_registry.Scan(ms_regKeys[index].path, "%f", out) != 0;
}

bool COption::ScanReg_S(int index, char* out) {
	if (index < 0 || index >= RegKeyCount || !strlen(ms_regKeys[index].path)) {
		return false;
	}
	return this->m_registry.Scan(ms_regKeys[index].path, "%s", out) != 0;
}

void COption::WriteReg(int index, const char* str) {
	if (index < 0 || index >= RegKeyCount || !strlen(ms_regKeys[index].path)) {
		return;
	}
	this->m_registry.Write(ms_regKeys[index].path, str);
}

bool COption::WriteReg_I(int index, int value) {
	if (index < 0 || index >= RegKeyCount || !strlen(ms_regKeys[index].path)) {
		return false;
	}
	char str[256];
	sprintf_s(str, _countof(str), "%d", value);
	this->m_registry.Write(ms_regKeys[index].path, str);
	return true;
}

bool COption::WriteReg_F(int index, float value) {
	if (index < 0 || index >= RegKeyCount || !strlen(ms_regKeys[index].path)) {
		return false;
	}
	char str[256];
	sprintf_s(str, _countof(str), "%.2f", value);
	this->m_registry.Write(ms_regKeys[index].path, str);
	return true;
}

bool COption::WriteReg_S(int index, const char* value) {
	if (index < 0 || index >= RegKeyCount || !strlen(ms_regKeys[index].path)) {
		return false;
	}
	char str[256];
	sprintf_s(str, _countof(str), "%s", value);
	this->m_registry.Write(ms_regKeys[index].path, str);
	return true;
}

void COption::SetGlobalView(WView* globalView) {
	this->m_globalView = globalView;
}

void COption::SetAudioManager(CSoundManager* audioManager) {
	this->m_soundManager = audioManager;
}

void COption::vApplyMipmap() {
	this->m_videoDev->Command(WDeviceMessageSetDdsRes, this->m_sOption1.m_vMipDDSRes, 0);
	this->m_videoDev->Command(WDeviceMessageUseMipmap, this->m_sOption1.m_vUseMipmap, 0);
	this->m_videoDev->Command(WDeviceMessageMaxMipLevel, this->m_sOption1.m_vMaxMipLevel, 0);
	this->m_videoDev->Command(WDeviceMessageMipCreateFilter, this->m_sOption1.m_vMipMapCreateFilter, 0);
}

void COption::vSetMinimumScreenSize(int gScreen, int width, int height) {
	this->m_sOption1.m_vGameScreen = gScreen;
	this->m_sOption1.m_vScreenWidth2 = width;
	this->m_sOption1.m_vScreenHeight2 = height;
}

void COption::vEnableShadow(bool enabled) {
	this->m_sOption1.m_vShadowEnabled = enabled;
}

void COption::vEnableShadowmap(bool enabled) {
	this->m_sOption1.m_vShadowmapEnabled = enabled;
}

bool COption::vIsWindowed() const {
	return this->m_sOption1.m_wWindowedMode != 0;
}

bool COption::vIsWideMode() const {
	return this->m_sOption1.m_vWideMode != 0;
}

bool COption::vIsShadowEnabled() const {
	return this->m_sOption1.m_vShadowEnabled != 0;
}

bool COption::vIsShadowmapEnabled() const {
	return this->m_sOption1.m_vShadowmapEnabled != 0;
}

bool COption::vIsReflectionEnabled() const {
	return this->m_sOption1.m_vReflectionEnabled != 0;
}

bool COption::vIsLodEnabled() const {
	return this->m_sOption1.m_vLodEnabled != 0;
}

bool COption::vIsNpcEnabled() {
	return this->m_sOption1.m_gNpcEnabled != 0;
}

bool COption::aIsMssEnabled() {
	return this->m_sOption1.m_aMssEnabled != 0;
}

bool COption::aIsMssHwSoundEnabled() const {
	return this->m_sOption1.m_aMssHwSoundEnabled != 0;
}

uint32_t COption::aGetMssSpeaker() const {
	return this->m_sOption1.m_aMssSpeaker;
}

void COption::aApplyAudioSetting() {
	// TODO: g_audio
	//g_audio->SetSpeakerType(this->m_sOption1.m_aMssSpeaker);
}

bool COption::gIsWhisperAllowed() const {
	return this->m_sOption1.m_gWhisper != 0;
}

bool COption::gIsInvitationAllowed() const {
	return this->m_sOption1.m_gInvitation != 0;
}

bool COption::gIsFriendConfirmNeeded() const {
	return this->m_sOption1.m_gFriendConfirm != 0;
}

bool COption::gIsChatWinTransparent() const {
	return this->m_sOption1.m_gTransChatWin != 0;
}

bool COption::gIsChatLineEntended() const {
	return this->m_sOption1.m_gExtendChatLine != 0;
}

bool COption::gIsPPLEnable() const {
	return this->m_sOption1.m_bPPLEnabled != 0;
}

void COption::gSetPPLEnable(bool enable) {
	this->m_sOption1.m_bPPLEnabled = enable;
}

void COption::gSetPPLSize(int value) {
	this->m_sOption1.m_gPPLSize = value;
}

void COption::gResetRestore() {
	this->m_sOption1.m_gRestore = 0;
	this->WriteReg_I(RegKeyRestore, 0);
}

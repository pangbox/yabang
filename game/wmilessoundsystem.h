#pragma once

#include <cstdint>
#include "mss.h"
#include "wlist.h"
#include <mmeapi.h>

class cFile;

class WMilesSoundSystem {
public:
	struct msample2d {
		void* data;
		uint32_t filesize;
		uint32_t refCount;
		int idle;
		uint32_t stream;
		char filename[1];
	};

	struct msample3d {
		void* data;
		uint32_t refCount;
		int idle;
		uint32_t stream;
		char filename[1];
	};

	struct mstream {
		HSAMPLE sample;
		struct msample2d* pmsample2d;
		HSAMPLE sample3d;
		msample3d* pmsample3d;
		HSTREAM stream;
		int32_t pos;
		float vol;
		float x, y, z;
		float maxDist;
		float minDist;
		DWORD type;
	};

	WMilesSoundSystem(HWND hWnd, const char* redistDir, bool enabled, int frequency, int bits, int channels);
	~WMilesSoundSystem();
	void SetReverb() const;
	bool InitMixer();
	void CloseMixer() const;
	void ClearAllStreams();
	void ClearAllSamples();
	int GetCPUpercent();
	int SpeakerType(MSS_MC_SPEC channel_spec);
	void SetBalance(int balance);
	void Activate(bool activate);
	void Resume(int handle, int loop);
	void Stop(int handle);
	void SetPitch(int handle, int playbackRate);
	int GetPitch(int handle);
	void SetVolume(int handle, float volume);
	void SetNearFar(int handle, float maxDist, float minDist);
	void SetPosition(int handle, float x, float y, float z);
	bool IsPlaying(int handle) const;
	int GetPosition(int handle) const;
	int GetLength(int length) const;
	void Release2D(msample2d* sample);
	void LoadSample3D(msample3d* sample);
	void Release3D(msample3d* sample3d);
	void Process();
	int GetEmptyHandle();
	void ClearIdleSample();
	HSAMPLE GetAllocateSample(int handle, bool interrupt);
	HSAMPLE GetAllocateSample3D(int handle, bool interrupt);
	bool Init(HWND hWnd, int frequency, int bits);
	void DestroySoundBuffer(int handle);
	void Play(int handle, bool interrupt, bool stop);
	void ClearAllSounds();
	msample2d* LoadSample2D(const char* filename);
	msample3d* LoadSample3D(const char* filename);
	void ClearIdleSound(int level);
	int Load(const char* filename, int type);

protected:
	static cFile* g_files[16];
	static int32_t g_offset[16];
	static unsigned Open_callback(const char* filename, unsigned* outHandle);
	static void Close_callback(unsigned handle);
	static int Seek_callback(unsigned handle, int pos, unsigned origin);
	static int Read_callback(unsigned handle, void* out, unsigned size);

	AIL m_ail;
	float m_defaultMaxDist;
	float m_defaultMinDist;
	float m_volumeL;
	float m_volumeR;
	bool m_flipX;
	bool m_noStop;
	HDIGDRIVER m_hDriver;
	WList<msample2d*> m_sample2dList{16, 16};
	HSAMPLE m_samples[16];
	DWORD m_sampleStreams[16];
	bool m_bInterrupt[16];
	WList<msample3d*> m_sample3dList{16, 16};
	HSAMPLE m_samples3d[16];
	DWORD m_sample3dStreams[16];
	bool m_bInterrupt3d[16];
	HSTREAM m_activeStreams[8];
	DWORD m_streamStreams[8];
	mstream m_streams[1024];
	DWORD m_streamIndex;
	HMIXER m_hMixer;
	MIXERCONTROLA m_mixerControl;
	MIXERLINEA m_mixer;
};

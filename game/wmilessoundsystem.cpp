#include "wmilessoundsystem.h"

#include "cfile.h"
#include "wresourcemanager.h"

cFile* WMilesSoundSystem::g_files[16]{};
int32_t WMilesSoundSystem::g_offset[16]{};

WMilesSoundSystem::WMilesSoundSystem(HWND hWnd, const char* redistDir, bool enabled, int frequency, int bits,
                                     int channels) {
	this->m_hDriver = nullptr;
	this->m_volumeL = 1.0;
	this->m_volumeR = 1.0;
	this->m_streamIndex = 0;
	this->m_hMixer = nullptr;
	this->m_defaultMaxDist = 0.0;
	this->m_defaultMinDist = 100.0;
	this->m_flipX = false;
	this->m_noStop = false;
	for (auto& stream : this->m_streams) {
		stream.sample = nullptr;
		stream.pmsample2d = nullptr;
		stream.sample3d = nullptr;
		stream.pmsample3d = nullptr;
		stream.stream = nullptr;
		stream.pos = 0;
		stream.type = 3;
	}
	for (auto& stream : this->m_activeStreams) {
		stream = nullptr;
	}
	for (auto& stream : this->m_streamStreams) {
		stream = 0;
	}
	for (auto& sample : this->m_samples) {
		sample = nullptr;
	}
	for (auto& stream : this->m_sample3dStreams) {
		stream = 0;
	}
	for (auto& sample : this->m_samples3d) {
		sample = nullptr;
	}
	if (redistDir) {
		m_ail.set_redist_directory(redistDir);
	}
	for (auto& file : g_files) {
		file = nullptr;
	}
	this->m_ail.set_file_callbacks(Open_callback, Close_callback, Seek_callback, Read_callback);
	if (!this->m_ail.startup()) {
		char buffer[256];
		char* err = this->m_ail.last_error();
		sprintf(buffer, "사운드를 초기화할 수 없습니다: %s", err);
		MessageBoxA(nullptr, buffer, "Miles Sound System", MB_OK);
	}
}

WMilesSoundSystem::~WMilesSoundSystem() {
	if (this->m_hMixer) {
		mixerClose(this->m_hMixer);
	}
	this->ClearAllSamples();
	this->ClearAllSounds();
	this->ClearAllStreams();
	for (int i = 0; i < 16; ++i) {
		if (!g_files[i]) {
			continue;
		}
		CloseCFile(g_files[i]);
		g_files[i] = nullptr;
	}
	if (this->m_hDriver) {
		this->m_ail.digital_handle_release(this->m_hDriver);
	}
	if (this->m_hDriver) {
		this->m_ail.close_digital_driver(this->m_hDriver);
	}
	this->m_ail.shutdown();
}

void WMilesSoundSystem::SetReverb() const {
	this->m_ail.set_digital_master_reverb(this->m_hDriver, 1.0f, 0.0f, 0.0f);
}

bool WMilesSoundSystem::InitMixer() {
	UINT numDevs = mixerGetNumDevs();
	if (!numDevs) {
		return false;
	}

	this->m_hMixer = nullptr;

	if (mixerOpen(&this->m_hMixer, 0, 0, 0, 0)) {
		return false;
	}

	memset(&this->m_mixer, 0, sizeof(this->m_mixer));
	memset(&this->m_mixerControl, 0, sizeof(this->m_mixerControl));

	this->m_mixer.cbStruct = sizeof(MIXERLINE);
	this->m_mixer.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
	if (mixerGetLineInfoA(reinterpret_cast<HMIXEROBJ>(this->m_hMixer), &this->m_mixer,
	                      MIXER_GETLINEINFOF_COMPONENTTYPE)) {
		return false;
	}

	MIXERLINECONTROLSA mixerLineControls;
	mixerLineControls.dwLineID = this->m_mixer.dwLineID;
	mixerLineControls.pamxctrl = &this->m_mixerControl;
	mixerLineControls.pamxctrl->cbStruct = sizeof(MIXERCONTROLA);
	mixerLineControls.cbStruct = sizeof(MIXERLINECONTROLSA);
	mixerLineControls.dwControlID = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mixerLineControls.cControls = 1;
	mixerLineControls.cbmxctrl = sizeof(MIXERCONTROLA);
	if (mixerGetLineControlsA(reinterpret_cast<HMIXEROBJ>(this->m_hMixer), &mixerLineControls,
	                          MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE)) {
		return false;
	}
	return true;
}

void WMilesSoundSystem::CloseMixer() const {
	if (this->m_hMixer) {
		mixerClose(this->m_hMixer);
	}
}

void WMilesSoundSystem::ClearAllStreams() {
	for (int i = 0; i < 8; i++) {
		if (this->m_activeStreams[i]) {
			this->m_ail.close_stream(this->m_activeStreams[i]);
			if (this->m_streamStreams[i]) {
				this->m_streams[this->m_streamStreams[i]].stream = nullptr;
				this->m_streams[this->m_streamStreams[i]].pos = 0;
			}
			this->m_streamStreams[i] = 0;
		}
	}
}

void WMilesSoundSystem::ClearAllSamples() {
	for (int i = 0; i < 16; i++) {
		if (!this->m_samples[i]) {
			continue;
		}
		this->m_ail.stop_sample(this->m_samples[i]);
		this->m_ail.release_sample_handle(this->m_samples[i]);
		this->m_samples[i] = nullptr;
		if (this->m_sampleStreams[i] > 0) {
			this->m_streams[this->m_sampleStreams[i]].sample = nullptr;
			this->m_sampleStreams[i] = 0;
		}
	}
	memset(this->m_samples, 0, sizeof(this->m_samples));

	for (int i = 0; i < 16; i++) {
		if (!this->m_samples3d[i]) {
			continue;
		}
		this->m_ail.stop_sample(this->m_samples3d[i]);
		this->m_ail.release_sample_handle(this->m_samples3d[i]);
		this->m_samples3d[i] = nullptr;
		if (this->m_sample3dStreams[i] > 0) {
			this->m_streams[this->m_sample3dStreams[i]].sample3d = nullptr;
			this->m_sample3dStreams[i] = 0;
		}
	}
	memset(this->m_samples3d, 0, sizeof(this->m_samples3d));
}

int WMilesSoundSystem::GetCPUpercent() {
	if (!this->m_hDriver) {
		return 0;
	}
	return this->m_ail.digital_CPU_percent(this->m_hDriver);
}

int WMilesSoundSystem::SpeakerType(MSS_MC_SPEC channel_spec) {
	float falloffPower = 0.0f;
	int32_t nPhysicalChannels;
	int32_t nLogicalChannels;

	switch (channel_spec) {
		case 0:
			nPhysicalChannels = 2;
			nLogicalChannels = 2;
			channel_spec = MSS_MC_STEREO;
			break;
		case 1:
			nPhysicalChannels = 2;
			nLogicalChannels = 2;
			channel_spec = MSS_MC_HEADPHONES;
			break;
		case 2:
			nPhysicalChannels = 2;
			nLogicalChannels = 3;
			channel_spec = MSS_MC_DOLBY_SURROUND;
			break;
		case 3:
			nPhysicalChannels = 4;
			nLogicalChannels = 4;
			channel_spec = MSS_MC_40_DISCRETE;
			break;
		case 4:
			nPhysicalChannels = 6;
			nLogicalChannels = 6;
			channel_spec = MSS_MC_51_DISCRETE;
			break;
		case 5:
			nPhysicalChannels = 8;
			nLogicalChannels = 8;
			channel_spec = MSS_MC_71_DISCRETE;
			break;
		default:
			break;
	}

	void* speakers = this->m_ail.speaker_configuration(this->m_hDriver, &nPhysicalChannels, &nLogicalChannels,
	                                                   &falloffPower, &channel_spec);
	return this->m_ail.set_speaker_configuration(this->m_hDriver, speakers, 0, 0.0f);
}

void WMilesSoundSystem::SetBalance(int balance) {
	if (balance >= 128) {
		this->m_volumeL = balance <= 256 ? (256.0f - static_cast<float>(balance)) / 128.0f : 1.0f;
		this->m_volumeR = 1.0;
	} else {
		this->m_volumeL = 1.0;
		this->m_volumeR = balance >= 0 ? static_cast<float>(balance) / 128.0f : 0.0f;
	}
}

void WMilesSoundSystem::Activate(bool activate) {}

void WMilesSoundSystem::Resume(int handle, int loop) {
	if (this->m_streams[handle].type != 2 || !this->m_streams[handle].stream) {
		return;
	}

	this->m_ail.set_stream_loop_count(this->m_streams[handle].stream, loop == 0);
	this->m_streams[handle].sample = this->m_ail.stream_sample_handle(this->m_streams[handle].stream);
	this->m_ail.set_sample_volume_pan(this->m_streams[handle].sample, this->m_streams[handle].vol, 0.5);
	this->m_ail.set_stream_position(this->m_streams[handle].stream, this->m_streams[handle].pos);
	this->m_ail.start_stream(this->m_streams[handle].stream);
}

void WMilesSoundSystem::Stop(int handle) {
	switch (this->m_streams[handle].type) {
		case 2:
			this->m_ail.pause_stream(this->m_streams[handle].stream, 1);
			this->m_streams[handle].pos = this->m_ail.stream_position(this->m_streams[handle].stream);
			break;
		case 1:
			if (this->m_streams[handle].sample3d) {
				break;
			}
			for (int i = 0; i < 16; i++) {
				if (this->m_sample3dStreams[i] != handle) {
					continue;
				}
				this->m_sample3dStreams[i] = 0;
				this->m_ail.stop_sample(this->m_samples3d[i]);
			}
		case 0:
			if (!this->m_streams[handle].sample) {
				break;
			}
			for (int i = 0; i < 16; i++) {
				if (this->m_sampleStreams[i] != handle) {
					continue;
				}
				this->m_sampleStreams[i] = 0;
				this->m_ail.stop_sample(this->m_samples[i]);
			}
			this->m_streams[handle].sample = nullptr;
		default:
			break;
	}
}

void WMilesSoundSystem::SetPitch(int handle, int playbackRate) {
	this->m_ail.set_sample_playback_rate(this->m_streams[handle].sample, playbackRate);
}

int WMilesSoundSystem::GetPitch(int handle) {
	return this->m_ail.sample_playback_rate(this->m_streams[handle].sample);
}

void WMilesSoundSystem::SetVolume(int handle, float volume) {
	switch (this->m_streams[handle].type) {
		case 0:
			volume *= this->m_volumeL;
			this->m_streams[handle].vol = volume;
			if (!this->m_streams[handle].sample)
				return;
			this->m_ail.set_sample_volume_pan(this->m_streams[handle].sample, volume, 0.5);
			return;
		case 1:
			volume *= this->m_volumeR;
			this->m_streams[handle].vol = volume;
			if (!this->m_streams[handle].sample3d)
				return;
			this->m_ail.set_sample_volume_pan(this->m_streams[handle].sample3d, volume, 0.5);
			return;
		case 2:
			this->m_streams[handle].vol = volume * this->m_volumeL;
			if (!this->m_streams[handle].stream) {
				return;
			}
			this->m_ail.set_sample_volume_pan(this->m_ail.stream_sample_handle(this->m_streams[handle].stream),
			                                  this->m_streams[handle].vol, 0.5);
	}
}

void WMilesSoundSystem::SetNearFar(int handle, float maxDist, float minDist) {
	this->m_streams[handle].maxDist = maxDist;
	this->m_streams[handle].minDist = minDist;
	if (!this->m_streams[handle].sample3d) {
		return;
	}
	this->m_ail.set_sample_3D_distances(this->m_streams[handle].sample3d, maxDist, minDist, 0);
}

void WMilesSoundSystem::SetPosition(int handle, float x, float y, float z) {
	if (this->m_flipX) {
		x = -x;
	}
	this->m_streams[handle].x = x;
	this->m_streams[handle].y = y;
	this->m_streams[handle].z = z;
	if (!this->m_streams[handle].sample3d) {
		return;
	}
	this->m_ail.set_sample_3D_position(this->m_streams[handle].sample3d, x, y, z);
}

bool WMilesSoundSystem::IsPlaying(int handle) const {
	HSTREAM stream;
	int streamStatus;
	HSAMPLE sample;

	switch (this->m_streams[handle].type) {
		case 0:
			sample = this->m_streams[handle].sample;
			return sample && this->m_ail.sample_status(sample) == 4;
		case 1:
			sample = this->m_streams[handle].sample3d;
			return sample && this->m_ail.sample_status(sample) == 4;
		case 2:
			stream = this->m_streams[handle].stream;
			if (!stream) {
				return false;
			}
			streamStatus = this->m_ail.stream_status(stream);
			return streamStatus != 2 && streamStatus != 8;
		default:
			return false;
	}
}

int WMilesSoundSystem::GetPosition(int handle) const {
	switch (this->m_streams[handle].type) {
		case 0:
			if (this->m_streams[handle].sample && this->m_ail.sample_status(this->m_streams[handle].sample) == 4) {
				return this->m_ail.sample_position(this->m_streams[handle].sample);
			}
			return -1;
		case 1:
			if (this->m_ail.sample_status(this->m_streams[handle].sample3d) == 4) {
				return this->m_ail.sample_position(this->m_streams[handle].sample3d);
			}
			return -1;
		case 2:
			if (this->m_ail.stream_status(this->m_streams[handle].stream) != 4) {
				return -1;
			}
			this->m_ail.stream_ms_position(this->m_streams[handle].stream, nullptr, &handle);
			return handle / 1000;
		default:
			return -1;
	}
}

int WMilesSoundSystem::GetLength(int length) const {
	int32_t sndtype;
	int32_t memory;
	int32_t datarate;

	switch (this->m_streams[length].type) {
		case 2:
			this->m_ail.stream_info(this->m_streams[length].stream, &datarate, &sndtype, &length, &memory);
			return length / datarate;
		default:
			return -1;
	}
}

void WMilesSoundSystem::Release2D(msample2d* sample) {
	--sample->refCount;
	sample->idle = 0;
}

void WMilesSoundSystem::LoadSample3D(msample3d* sample) {
	void* info;
	void* outData;

	const char* filename = sample->filename;
	uint32_t filesize = this->m_ail.file_size(sample->filename);
	sample->data = this->m_ail.file_read(filename, nullptr);
	switch (this->m_ail.file_type(sample->data, filesize)) {
		case 0xD:
			this->m_ail.decompress_ASI(sample->data, filesize, filename, &outData, nullptr, nullptr);
			this->m_ail.mem_free_lock(sample->data);
			sample->data = outData;
			break;
		case 0x2:
			this->m_ail.WAV_info(sample->data, &info);
			this->m_ail.decompress_ADPCM(&info, &outData, nullptr);
			this->m_ail.mem_free_lock(sample->data);
			sample->data = outData;
			break;
		default:
			this->m_ail.mem_free_lock(sample->data);
	}
}

void WMilesSoundSystem::Release3D(msample3d* sample3d) {
	--sample3d->refCount;
	sample3d->idle = 0;
}

void WMilesSoundSystem::Process() {}

int WMilesSoundSystem::GetEmptyHandle() {
	for (int i = 0; i < 1024; i++) {
		this->m_streamIndex = (m_streamIndex + 1) & 0x3FF;
		if (!this->m_streamIndex || this->m_streams[this->m_streamIndex].type != 3) {
			continue;
		}
		return this->m_streamIndex;
	}
	return 0;
}

void WMilesSoundSystem::ClearIdleSample() {
	for (int i = 0; i < 16; i++) {
		if (!this->m_samples[i]) {
			continue;
		}
		int status = this->m_ail.sample_status(this->m_samples[i]);
		if (status != 2 && status != 8) {
			continue;
		}
		if (this->m_sampleStreams[i] > 0) {
			if (this->m_streams[this->m_sampleStreams[i]].sample == this->m_samples[i])
				this->m_streams[this->m_sampleStreams[i]].sample = nullptr;
			this->m_sampleStreams[i] = 0;
		}
		this->m_ail.stop_sample(this->m_samples[i]);
		this->m_ail.release_sample_handle(this->m_samples[i]);
		this->m_samples[i] = nullptr;
	}

	for (int i = 0; i < 16; i++) {
		if (!this->m_samples3d[i]) {
			continue;
		}
		int status = this->m_ail.sample_status(this->m_samples3d[i]);
		if (status != 2 && status != 8) {
			continue;
		}
		if (this->m_sample3dStreams[i] > 0) {
			if (this->m_streams[this->m_sample3dStreams[i]].sample3d == this->m_samples3d[i])
				this->m_streams[this->m_sample3dStreams[i]].sample3d = nullptr;
			this->m_sample3dStreams[i] = 0;
		}
		this->m_ail.stop_sample(this->m_samples3d[i]);
		this->m_ail.release_sample_handle(this->m_samples3d[i]);
		this->m_samples3d[i] = nullptr;
	}
}

HSAMPLE WMilesSoundSystem::GetAllocateSample(int handle, bool interrupt) {
	int i;
	for (i = 0; i < 16; i++) {
		if (!this->m_samples[i]) {
			this->m_samples[i] = this->m_ail.allocate_sample_handle(this->m_hDriver);
			break;
		}
		int sampleStatus = this->m_ail.sample_status(this->m_samples[i]);
		if (sampleStatus == 2 || sampleStatus == 8) {
			break;
		}
	}

	if (i == 16 || !this->m_samples[i]) {
		for (i = 0; i < 16; i++) {
			if (this->m_samples[i] && this->m_bInterrupt[i]) {
				break;
			}
		}
		if (i == 16) {
			return nullptr;
		}
	}

	int sampleStreamIndex = this->m_sampleStreams[i];
	if (sampleStreamIndex > 0) {
		if (this->m_streams[sampleStreamIndex].sample == this->m_samples[i])
			this->m_streams[sampleStreamIndex].sample = nullptr;
	}
	this->m_sampleStreams[i] = handle;
	this->m_bInterrupt[i] = interrupt;
	this->m_streams[handle].sample = this->m_samples[i];
	this->m_ail.init_sample(this->m_samples3d[i], 2); // ?? this might be a bug
	return this->m_samples[i];
}

HSAMPLE WMilesSoundSystem::GetAllocateSample3D(int handle, bool interrupt) {
	int i;
	for (i = 0; i < 16; i++) {
		if (!this->m_samples3d[i]) {
			this->m_samples3d[i] = this->m_ail.allocate_sample_handle(this->m_hDriver);
			break;
		}
		int sampleStatus = this->m_ail.sample_status(this->m_samples3d[i]);
		if (sampleStatus == 2 || sampleStatus == 8) {
			break;
		}
	}

	if (i == 16 || !this->m_samples3d[i]) {
		for (i = 0; i < 16; i++) {
			if (this->m_samples3d[i] && this->m_bInterrupt3d[i]) {
				break;
			}
		}
		if (i == 16) {
			return nullptr;
		}
	}

	int sampleStreamIndex = this->m_sample3dStreams[i];
	if (sampleStreamIndex > 0) {
		if (this->m_streams[sampleStreamIndex].sample3d == this->m_samples3d[i])
			this->m_streams[sampleStreamIndex].sample3d = nullptr;
	}
	this->m_sample3dStreams[i] = handle;
	this->m_bInterrupt3d[i] = interrupt;
	this->m_streams[handle].sample3d = this->m_samples3d[i];
	return this->m_samples3d[i];
}

bool WMilesSoundSystem::Init(HWND hWnd, int frequency, int bits) {
	this->m_hDriver = this->m_ail.open_digital_driver(frequency, bits, 16, 1);
	if (!this->m_hDriver) {
		char buff[256];
		char* err = this->m_ail.last_error();
		sprintf(buff, "사운드 드라이버를 열 수 없습니다: %s", err);
		MessageBoxA(nullptr, buff, "Miles Sound System", MB_OK);
		return false;
	}
	this->m_ail.set_DirectSound_HWND(this->m_hDriver, hWnd);
	this->InitMixer();
	return true;
}

void WMilesSoundSystem::DestroySoundBuffer(int handle) {
	this->Stop(handle);

	switch (this->m_streams[handle].type) {
		case 0:
			this->m_streams[handle].pmsample2d->refCount--;
			this->m_streams[handle].pmsample2d->idle = 0;
			this->m_streams[handle].pmsample2d = nullptr;
			this->m_streams[handle].type = 3;
			break;
		case 1:
			this->m_streams[handle].pmsample3d->refCount--;
			this->m_streams[handle].pmsample3d->idle = 0;
			this->m_streams[handle].pmsample3d = nullptr;
			this->m_streams[handle].type = 3;
			break;
		case 2:

			break;
		default:
			this->m_streams[handle].type = 3;
			return;
	}

	if (!this->m_streams[handle].stream) {
		this->m_streams[handle].type = 3;
		return;
	}

	this->m_ail.close_stream(this->m_streams[handle].stream);

	int i;
	for (i = 0; i < 8; i++) {
		if (this->m_activeStreams[i] == this->m_streams[handle].stream) {
			break;
		}
		if (this->m_streamStreams[i] != handle) {
			continue;
		}
		this->m_ail.close_stream(this->m_activeStreams[i]);
	}

	this->m_activeStreams[i] = nullptr;
	if (this->m_streamStreams[i]) {
		this->m_streams[this->m_streamStreams[i]].stream = nullptr;
		this->m_streamStreams[i] = 0;
	}
	this->m_streams[handle].stream = nullptr;
	this->m_streams[handle].type = 3;
}

void WMilesSoundSystem::Play(int handle, bool interrupt, bool stop) {
	int oldStream;
	HSAMPLE sample;

	switch (this->m_streams[handle].type) {
		case 0:
			if (!this->m_streams[handle].pmsample2d->data) {
				break;
			}
			oldStream = this->m_streams[handle].pmsample2d->stream;
			this->m_streams[handle].pmsample2d->stream = handle;
			if (!this->m_noStop && stop
				&& !this->m_streams[oldStream].type
				&& this->m_streams[oldStream].pmsample2d == this->m_streams[handle].pmsample2d) {
				this->Stop(oldStream);
			}
			sample = this->GetAllocateSample(handle, !interrupt);
			if (!sample) {
				break;
			}
			this->m_ail.set_named_sample_file(
				sample,
				this->m_streams[handle].pmsample2d->filename,
				this->m_streams[handle].pmsample2d->data,
				this->m_streams[handle].pmsample2d->filesize,
				0);
			this->m_ail.set_sample_loop_count(sample, !interrupt);
			this->m_ail.set_sample_volume_pan(sample, this->m_streams[handle].vol, 0.5);
			this->m_ail.start_sample(sample);
			break;
		case 1:
			oldStream = this->m_streams[handle].pmsample3d->stream;
			this->m_streams[handle].pmsample3d->stream = handle;
			if (!this->m_noStop && stop
				&& this->m_streams[oldStream].type == 1
				&& this->m_streams[oldStream].pmsample3d == this->m_streams[handle].pmsample3d) {
				this->Stop(oldStream);
			}
			sample = this->GetAllocateSample3D(handle, !interrupt);
			if (!sample) {
				break;
			}
			this->m_ail.set_sample_file(sample, this->m_streams[handle].pmsample3d->data, 0);
			this->m_ail.set_sample_loop_count(sample, !interrupt);
			this->m_ail.set_sample_3D_distances(sample, this->m_streams[handle].maxDist,
			                                    this->m_streams[handle].minDist, 0);
			this->m_ail.set_sample_volume_pan(sample, this->m_streams[handle].vol, 0.5);
			this->m_ail.set_sample_3D_position(this->m_streams[handle].sample3d, this->m_streams[handle].x,
			                                   this->m_streams[handle].y, this->m_streams[handle].z);
			this->m_ail.start_sample(sample);
			break;
		case 2:
			if (this->m_streams[handle].stream) {
				this->m_ail.set_stream_loop_count(this->m_streams[handle].stream, !interrupt);
				sample = this->m_ail.stream_sample_handle(this->m_streams[handle].stream);
				this->m_ail.set_sample_volume_pan(sample, this->m_streams[handle].vol, 0.5);
				this->m_ail.start_stream(this->m_streams[handle].stream);
			}
			break;
	}
}

void WMilesSoundSystem::ClearAllSounds() {
	for (auto* i = this->m_sample2dList.Start(); i != nullptr; i = this->m_sample2dList.Next()) {
		this->m_ail.mem_free_lock(i->data);
		delete[] i;
	}
	this->m_sample2dList.Reset();
	for (auto* i = this->m_sample3dList.Start(); i != nullptr; i = this->m_sample3dList.Next()) {
		this->m_ail.mem_free_lock(i->data);
		delete[] i;
	}
	this->m_sample3dList.Reset();
}

WMilesSoundSystem::msample2d* WMilesSoundSystem::LoadSample2D(const char* filename) {
	msample2d* sample2d = this->m_sample2dList.Find(filename);
	if (!sample2d) {
		sample2d = reinterpret_cast<msample2d*>(new char[sizeof(msample2d) + strlen(filename)]);
		sample2d->filesize = this->m_ail.file_size(filename);
		sample2d->data = sample2d->filesize <= 0 ? nullptr : this->m_ail.file_read(filename, nullptr);
		sample2d->refCount = 0;
		sample2d->stream = 0;
		strcpy(sample2d->filename, filename);
		this->m_sample2dList.AddItem(sample2d, sample2d->filename, false);
	}
	++sample2d->refCount;
	return sample2d;
}

WMilesSoundSystem::msample3d* WMilesSoundSystem::LoadSample3D(const char* filename) {
	msample3d* sample3d = this->m_sample3dList.Find(filename);
	if (!sample3d) {
		sample3d = reinterpret_cast<msample3d*>(new char[sizeof(sample3d) + strlen(filename)]);
		sample3d->refCount = 0;
		sample3d->idle = 0;
		sample3d->stream = 0;
		strcpy(sample3d->filename, filename);
		LoadSample3D(sample3d);
		this->m_sample3dList.AddItem(sample3d, sample3d->filename, false);
	}
	++sample3d->refCount;
	return sample3d;
}

void WMilesSoundSystem::ClearIdleSound(int level) {
	WList<msample2d*> sample2dIdle{16, 16};
	WList<msample3d*> sample3dIdle{16, 16};

	for (auto* i = this->m_sample2dList.Start(); i != nullptr; i = this->m_sample2dList.Next()) {
		if (!i->refCount && ++i->idle >= level + 1) {
			sample2dIdle.AddItem(i, nullptr, false);
		}
	}

	for (auto* i = this->m_sample3dList.Start(); i != nullptr; i = this->m_sample3dList.Next()) {
		if (!i->refCount && ++i->idle >= level + 1) {
			sample3dIdle.AddItem(i, nullptr, false);
		}
	}

	for (auto* i = sample2dIdle.Start(); i != nullptr; i = sample2dIdle.Next()) {
		this->m_sample2dList.DelItem(i);
		this->m_ail.mem_free_lock(i->data);
		delete[] i;
	}

	for (auto* i = sample3dIdle.Start(); i != nullptr; i = sample3dIdle.Next()) {
		this->m_sample3dList.DelItem(i);
		this->m_ail.mem_free_lock(i->data);
		delete[] i;
	}
}

int WMilesSoundSystem::Load(const char* filename, int type) {
	uint32_t i;
	HSTREAM stream;
	HSAMPLE sample;
	int rate;
	msample3d* pmsample3d;
	msample2d* pmsample2d;

	int handle = this->GetEmptyHandle();
	if (handle <= 0) {
		return 0;
	}

	switch (type) {
		case 0:
			pmsample2d = this->LoadSample2D(filename);
			if (!pmsample2d) {
				return 0;
			}
			this->m_streams[handle].pmsample2d = pmsample2d;
			this->m_streams[handle].sample = nullptr;
			this->m_streams[handle].type = 0;
			this->SetVolume(handle, 1.0f);
			return handle;
		case 1:
			pmsample3d = this->LoadSample3D(filename);
			if (!pmsample3d) {
				return 0;
			}
			this->m_streams[handle].pmsample3d = pmsample3d;
			this->m_streams[handle].sample3d = nullptr;
			this->m_streams[handle].maxDist = this->m_defaultMaxDist;
			this->m_streams[handle].minDist = this->m_defaultMinDist;
			this->m_streams[handle].type = 1;
			this->SetVolume(handle, 1.0f);
			return handle;
		case 2:
			for (i = 0; i < 8; i++) {
				if (!this->m_activeStreams[i]) {
					break;
				}
				int status = this->m_ail.stream_status(this->m_activeStreams[i]);
				if (status == 2 || status == 8) {
					break;
				}
			}
			if (i == 8) {
				return 0;
			}
			if (this->m_activeStreams[i]) {
				if (this->m_streams[this->m_streamStreams[i]].stream == this->m_activeStreams[i]) {
					this->m_streams[this->m_streamStreams[i]].stream = nullptr;
				}
				this->m_ail.close_stream(this->m_activeStreams[i]);
				this->m_activeStreams[i] = nullptr;
			}
			stream = this->m_ail.open_stream(this->m_hDriver, filename, 0);
			if (!stream) {
				return 0;
			}
			this->m_activeStreams[i] = stream;
			this->m_streamStreams[i] = handle;
			sample = this->m_ail.stream_sample_handle(stream);
			rate = this->m_ail.sample_playback_rate(sample);
			this->m_ail.set_stream_user_data(stream, i, rate);
			this->m_streams[handle].stream = stream;
			this->m_streams[handle].type = 2;
			this->SetVolume(handle, 1.0);
			return handle;
	}
	return 0;
}

void WMilesSoundSystem::SetDistLimit(float maxDist, float minDist) {
	m_defaultMaxDist = maxDist;
	m_defaultMinDist = minDist;
}

unsigned int WMilesSoundSystem::Open_callback(const char* filename, unsigned int* outHandle) {
	for (size_t i = 1; i < _countof(g_files); i++) {
		if (g_files[i]) {
			continue;
		}
		cFile* f = g_resourceManager->GetCFile(filename, 0xFFFF);
		if (!f) {
			return 0;
		}
		g_files[i] = f;
		g_offset[i] = -1;
		*outHandle = i;
		return 1;
	}
	return 0;
}

void WMilesSoundSystem::Close_callback(unsigned int handle) {
	CloseCFile(g_files[handle]);
	g_files[handle] = nullptr;
}

int WMilesSoundSystem::Seek_callback(unsigned int handle, int pos, unsigned int origin) {
	switch (origin) {
		case SEEK_SET:
			return g_offset[handle] = pos;
		default:
		case SEEK_CUR:
			return g_offset[handle] = g_files[handle]->Tell() + pos;
		case SEEK_END:
			return g_offset[handle] = g_files[handle]->Length() + pos;
	}
}

int WMilesSoundSystem::Read_callback(unsigned int handle, void* out, unsigned int size) {
	int offset = g_offset[handle];
	if (offset != -1) {
		g_files[handle]->Seek(offset, 0);
		g_offset[handle] = -1;
	}
	return g_files[handle]->Read(out, size);
}

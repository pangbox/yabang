#include "mss.h"

#include <cstdlib>

AIL::AIL() {
	this->mss32 = LoadLibrary(TEXT("mss32.dll"));
	if (!this->mss32) {
		abort();
	}
	last_error = (PFN_AIL_last_error)GetProcAddress(this->mss32, "AIL_last_error");
	set_redist_directory = (PFN_AIL_set_redist_directory)GetProcAddress(this->mss32, "AIL_set_redist_directory");
	set_file_callbacks = (PFN_AIL_set_file_callbacks)GetProcAddress(this->mss32, "AIL_set_file_callbacks");
	startup = (PFN_AIL_startup)GetProcAddress(this->mss32, "AIL_startup");
	open_digital_driver = (PFN_AIL_open_digital_driver)GetProcAddress(this->mss32, "AIL_open_digital_driver");
	set_DirectSound_HWND = (PFN_AIL_set_DirectSound_HWND)GetProcAddress(this->mss32, "AIL_set_DirectSound_HWND");
	close_stream = (PFN_AIL_close_stream)GetProcAddress(this->mss32, "AIL_close_stream");
	pause_stream = (PFN_AIL_pause_stream)GetProcAddress(this->mss32, "AIL_pause_stream");
	stream_position = (PFN_AIL_stream_position)GetProcAddress(this->mss32, "AIL_stream_position");
	set_stream_loop_count = (PFN_AIL_set_stream_loop_count)GetProcAddress(this->mss32, "AIL_set_stream_loop_count");
	stream_sample_handle = (PFN_AIL_stream_sample_handle)GetProcAddress(this->mss32, "AIL_stream_sample_handle");
	stream_info = (PFN_AIL_stream_info)GetProcAddress(this->mss32, "AIL_stream_info");
	set_sample_volume_pan = (PFN_AIL_set_sample_volume_pan)GetProcAddress(this->mss32, "AIL_set_sample_volume_pan");
	start_stream = (PFN_AIL_start_stream)GetProcAddress(this->mss32, "AIL_start_stream");
	set_sample_file = (PFN_AIL_set_sample_file)GetProcAddress(this->mss32, "AIL_set_sample_file");
	set_sample_3D_distances = (PFN_AIL_set_sample_3D_distances)GetProcAddress(this->mss32, "AIL_set_sample_3D_distances");
	set_sample_3D_position = (PFN_AIL_set_sample_3D_position)GetProcAddress(this->mss32, "AIL_set_sample_3D_position");
	start_sample = (PFN_AIL_start_sample)GetProcAddress(this->mss32, "AIL_start_sample");
	set_named_sample_file = (PFN_AIL_set_named_sample_file)GetProcAddress(this->mss32, "AIL_set_named_sample_file");
	set_digital_master_reverb = (PFN_AIL_set_digital_master_reverb)GetProcAddress(this->mss32, "AIL_set_digital_master_reverb");
	stop_sample = (PFN_AIL_stop_sample)GetProcAddress(this->mss32, "AIL_stop_sample");
	set_sample_playback_rate = (PFN_AIL_set_sample_playback_rate)GetProcAddress(this->mss32, "AIL_set_sample_playback_rate");
	release_sample_handle = (PFN_AIL_release_sample_handle)GetProcAddress(this->mss32, "AIL_release_sample_handle");
	decompress_ADPCM = (PFN_AIL_decompress_ADPCM)GetProcAddress(this->mss32, "AIL_decompress_ADPCM");
	digital_CPU_percent = (PFN_AIL_digital_CPU_percent)GetProcAddress(this->mss32, "AIL_digital_CPU_percent");
	speaker_configuration = (PFN_AIL_speaker_configuration)GetProcAddress(this->mss32, "AIL_speaker_configuration");
	set_speaker_configuration = (PFN_AIL_set_speaker_configuration)GetProcAddress(this->mss32, "AIL_set_speaker_configuration");
	mem_free_lock = (PFN_AIL_mem_free_lock)GetProcAddress(this->mss32, "AIL_mem_free_lock");
	digital_handle_release = (PFN_AIL_digital_handle_release)GetProcAddress(this->mss32, "AIL_digital_handle_release");
	shutdown = (PFN_AIL_shutdown)GetProcAddress(this->mss32, "AIL_shutdown");
	WAV_info = (PFN_AIL_WAV_info)GetProcAddress(this->mss32, "AIL_WAV_info");
	decompress_ASI = (PFN_AIL_decompress_ASI)GetProcAddress(this->mss32, "AIL_decompress_ASI");
	file_type = (PFN_AIL_file_type)GetProcAddress(this->mss32, "AIL_file_type");
	file_read = (PFN_AIL_file_read)GetProcAddress(this->mss32, "AIL_file_read");
	file_size = (PFN_AIL_file_size)GetProcAddress(this->mss32, "AIL_file_size");
	init_sample = (PFN_AIL_init_sample)GetProcAddress(this->mss32, "AIL_init_sample");
	allocate_sample_handle = (PFN_AIL_allocate_sample_handle)GetProcAddress(this->mss32, "AIL_allocate_sample_handle");
	set_stream_position = (PFN_AIL_set_stream_position)GetProcAddress(this->mss32, "AIL_set_stream_position");
	sample_playback_rate = (PFN_AIL_sample_playback_rate)GetProcAddress(this->mss32, "AIL_sample_playback_rate");
	sample_status = (PFN_AIL_sample_status)GetProcAddress(this->mss32, "AIL_sample_status");
	stream_status = (PFN_AIL_stream_status)GetProcAddress(this->mss32, "AIL_stream_status");
	sample_position = (PFN_AIL_sample_position)GetProcAddress(this->mss32, "AIL_sample_position");
	stream_ms_position = (PFN_AIL_stream_ms_position)GetProcAddress(this->mss32, "AIL_stream_ms_position");
}

AIL::~AIL() {
	FreeLibrary(this->mss32);
}

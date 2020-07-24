#include "mss.h"

#include <cstdlib>

#ifdef _WIN64
constexpr TCHAR g_mssLib[] = TEXT("mss64.dll");
#elif _WIN32
constexpr TCHAR g_mssLib[] = TEXT("mss32.dll");
#endif

AIL::AIL() {
	#ifdef _WIN32
	this->mss = LoadLibrary(g_mssLib);
	if (!this->mss) {
		throw MssNotFound();
	}
	last_error = reinterpret_cast<PFN_AIL_last_error>(GetProcAddress(this->mss, "AIL_last_error"));
	set_redist_directory = reinterpret_cast<PFN_AIL_set_redist_directory>(GetProcAddress(
		this->mss, "AIL_set_redist_directory"));
	set_file_callbacks = reinterpret_cast<PFN_AIL_set_file_callbacks>(GetProcAddress(
		this->mss, "AIL_set_file_callbacks"));
	startup = reinterpret_cast<PFN_AIL_startup>(GetProcAddress(this->mss, "AIL_startup"));
	open_digital_driver = reinterpret_cast<PFN_AIL_open_digital_driver>(GetProcAddress(
		this->mss, "AIL_open_digital_driver"));
	set_DirectSound_HWND = reinterpret_cast<PFN_AIL_set_DirectSound_HWND>(GetProcAddress(
		this->mss, "AIL_set_DirectSound_HWND"));
	close_stream = reinterpret_cast<PFN_AIL_close_stream>(GetProcAddress(this->mss, "AIL_close_stream"));
	pause_stream = reinterpret_cast<PFN_AIL_pause_stream>(GetProcAddress(this->mss, "AIL_pause_stream"));
	stream_position = reinterpret_cast<PFN_AIL_stream_position>(GetProcAddress(this->mss, "AIL_stream_position"));
	set_stream_loop_count = reinterpret_cast<PFN_AIL_set_stream_loop_count>(GetProcAddress(
		this->mss, "AIL_set_stream_loop_count"));
	stream_sample_handle = reinterpret_cast<PFN_AIL_stream_sample_handle>(GetProcAddress(
		this->mss, "AIL_stream_sample_handle"));
	stream_info = reinterpret_cast<PFN_AIL_stream_info>(GetProcAddress(this->mss, "AIL_stream_info"));
	set_sample_volume_pan = reinterpret_cast<PFN_AIL_set_sample_volume_pan>(GetProcAddress(
		this->mss, "AIL_set_sample_volume_pan"));
	start_stream = reinterpret_cast<PFN_AIL_start_stream>(GetProcAddress(this->mss, "AIL_start_stream"));
	set_sample_file = reinterpret_cast<PFN_AIL_set_sample_file>(GetProcAddress(this->mss, "AIL_set_sample_file"));
	set_sample_3D_distances = reinterpret_cast<PFN_AIL_set_sample_3D_distances>(GetProcAddress(
		this->mss, "AIL_set_sample_3D_distances"));
	set_sample_3D_position = reinterpret_cast<PFN_AIL_set_sample_3D_position>(GetProcAddress(
		this->mss, "AIL_set_sample_3D_position"));
	start_sample = reinterpret_cast<PFN_AIL_start_sample>(GetProcAddress(this->mss, "AIL_start_sample"));
	set_named_sample_file = reinterpret_cast<PFN_AIL_set_named_sample_file>(GetProcAddress(
		this->mss, "AIL_set_named_sample_file"));
	set_digital_master_reverb = reinterpret_cast<PFN_AIL_set_digital_master_reverb>(GetProcAddress(
		this->mss, "AIL_set_digital_master_reverb"));
	stop_sample = reinterpret_cast<PFN_AIL_stop_sample>(GetProcAddress(this->mss, "AIL_stop_sample"));
	set_sample_playback_rate = reinterpret_cast<PFN_AIL_set_sample_playback_rate>(GetProcAddress(
		this->mss, "AIL_set_sample_playback_rate"));
	release_sample_handle = reinterpret_cast<PFN_AIL_release_sample_handle>(GetProcAddress(
		this->mss, "AIL_release_sample_handle"));
	decompress_ADPCM = reinterpret_cast<PFN_AIL_decompress_ADPCM>(GetProcAddress(this->mss, "AIL_decompress_ADPCM"));
	digital_CPU_percent = reinterpret_cast<PFN_AIL_digital_CPU_percent>(GetProcAddress(
		this->mss, "AIL_digital_CPU_percent"));
	speaker_configuration = reinterpret_cast<PFN_AIL_speaker_configuration>(GetProcAddress(
		this->mss, "AIL_speaker_configuration"));
	set_speaker_configuration = reinterpret_cast<PFN_AIL_set_speaker_configuration>(GetProcAddress(
		this->mss, "AIL_set_speaker_configuration"));
	mem_free_lock = reinterpret_cast<PFN_AIL_mem_free_lock>(GetProcAddress(this->mss, "AIL_mem_free_lock"));
	digital_handle_release = reinterpret_cast<PFN_AIL_digital_handle_release>(GetProcAddress(
		this->mss, "AIL_digital_handle_release"));
	shutdown = reinterpret_cast<PFN_AIL_shutdown>(GetProcAddress(this->mss, "AIL_shutdown"));
	WAV_info = reinterpret_cast<PFN_AIL_WAV_info>(GetProcAddress(this->mss, "AIL_WAV_info"));
	decompress_ASI = reinterpret_cast<PFN_AIL_decompress_ASI>(GetProcAddress(this->mss, "AIL_decompress_ASI"));
	file_type = reinterpret_cast<PFN_AIL_file_type>(GetProcAddress(this->mss, "AIL_file_type"));
	file_read = reinterpret_cast<PFN_AIL_file_read>(GetProcAddress(this->mss, "AIL_file_read"));
	file_size = reinterpret_cast<PFN_AIL_file_size>(GetProcAddress(this->mss, "AIL_file_size"));
	init_sample = reinterpret_cast<PFN_AIL_init_sample>(GetProcAddress(this->mss, "AIL_init_sample"));
	allocate_sample_handle = reinterpret_cast<PFN_AIL_allocate_sample_handle>(GetProcAddress(
		this->mss, "AIL_allocate_sample_handle"));
	set_stream_position = reinterpret_cast<PFN_AIL_set_stream_position>(GetProcAddress(
		this->mss, "AIL_set_stream_position"));
	sample_playback_rate = reinterpret_cast<PFN_AIL_sample_playback_rate>(GetProcAddress(
		this->mss, "AIL_sample_playback_rate"));
	sample_status = reinterpret_cast<PFN_AIL_sample_status>(GetProcAddress(this->mss, "AIL_sample_status"));
	stream_status = reinterpret_cast<PFN_AIL_stream_status>(GetProcAddress(this->mss, "AIL_stream_status"));
	sample_position = reinterpret_cast<PFN_AIL_sample_position>(GetProcAddress(this->mss, "AIL_sample_position"));
	stream_ms_position = reinterpret_cast<PFN_AIL_stream_ms_position>(GetProcAddress(
		this->mss, "AIL_stream_ms_position"));
	set_sample_loop_count = reinterpret_cast<PFN_AIL_set_sample_loop_count>(GetProcAddress(
		this->mss, "AIL_set_sample_loop_count"));
	open_stream = reinterpret_cast<PFN_AIL_open_stream>(GetProcAddress(this->mss, "AIL_open_stream"));
	set_stream_user_data = reinterpret_cast<PFN_AIL_set_stream_user_data>(GetProcAddress(
		this->mss, "AIL_set_stream_user_data"));
	close_digital_driver = reinterpret_cast<PFN_AIL_close_digital_driver>(GetProcAddress(
		this->mss, "AIL_close_digital_driver"));
	#endif
}

AIL::~AIL() {
	FreeLibrary(this->mss);
}

#pragma once

/*
 * We do not have access to the Miles Sound System SDK, but we want to
 * be able to use MSS (at least temporarily.) This shim contains enough code
 * to load MSS and use functionality needed by PangYa. We should aim to
 * replace all of this with open source alternatives.
 *
 * Most importantly, Yabang must NEVER be redistributed with copies of MSS.
 */

#include <cstdint>
#include <windows.h>

// Opaque pointer types.
typedef void* HSTREAM;
typedef void* HSAMPLE;
typedef void* HDIGDRIVER;

// Enumerations
typedef enum
{
	MSS_MC_INVALID = 0,
	MSS_MC_MONO = 1,   
	MSS_MC_STEREO = 2,
	MSS_MC_USE_SYSTEM_CONFIG = 0x10,
	MSS_MC_HEADPHONES = 0x20,
	MSS_MC_DOLBY_SURROUND = 0x30,
	MSS_MC_SRS_CIRCLE_SURROUND = 0x40,
	MSS_MC_40_DTS = 0x48,
	MSS_MC_40_DISCRETE = 0x50,
	MSS_MC_51_DTS = 0x58,
	MSS_MC_51_DISCRETE = 0x60,
	MSS_MC_61_DISCRETE = 0x70,
	MSS_MC_71_DISCRETE = 0x80,
	MSS_MC_81_DISCRETE = 0x90,
	MSS_MC_DIRECTSOUND3D = 0xA0,
	MSS_MC_EAX2 = 0xC0,
	MSS_MC_EAX3 = 0xD0,
	MSS_MC_EAX4 = 0xE0
} MSS_MC_SPEC;

// Function pointer types.
typedef char* (__stdcall* PFN_AIL_last_error)();
typedef void (__stdcall* PFN_AIL_set_redist_directory)(const char* redist_dir);
typedef void (__stdcall* PFN_AIL_set_file_callbacks)(void* open, void* close, void* seek, void* read);
typedef int (__stdcall* PFN_AIL_startup)();
typedef HDIGDRIVER (__stdcall *PFN_AIL_open_digital_driver)(uint32_t frequency, int32_t bits, int32_t channel,
                                                            uint32_t flags);
typedef uint32_t (__stdcall *PFN_AIL_set_DirectSound_HWND)(HDIGDRIVER dig, HWND hwnd);
typedef void (__stdcall* PFN_AIL_close_stream)(HSTREAM stream);
typedef void (__stdcall* PFN_AIL_pause_stream)(HSTREAM stream, int32_t pause);
typedef uint32_t (__stdcall* PFN_AIL_stream_position)(HSTREAM stream);
typedef uint32_t (__stdcall* PFN_AIL_set_stream_loop_count)(HSTREAM stream, int32_t loop);
typedef HSAMPLE (__stdcall* PFN_AIL_stream_sample_handle)(HSTREAM stream);
typedef void (__stdcall* PFN_AIL_stream_info)(HSTREAM stream, int32_t* datarate, int32_t* sndtype, int32_t* length,
                                              int32_t* memory);
typedef uint32_t (__stdcall* PFN_AIL_set_sample_volume_pan)(HSAMPLE stream, float volume, float pan);
typedef void (__stdcall* PFN_AIL_start_stream)(HSTREAM stream);
typedef int32_t (__stdcall* PFN_AIL_set_sample_file)(HSAMPLE samploe, const void* data, int unknown);
typedef void (__stdcall* PFN_AIL_set_sample_3D_distances)(HSAMPLE sample, float max_dist, float min_dist,
                                                          int32_t unknown);
typedef void (__stdcall* PFN_AIL_set_sample_3D_position)(HSAMPLE sample, float x, float y, float z);
typedef void (__stdcall* PFN_AIL_start_sample)(HSAMPLE sample);
typedef void (__stdcall* PFN_AIL_set_named_sample_file)(HSAMPLE sample, const char* filename, const void* data,
                                                        uint32_t filesize, int unknown);
typedef void (__stdcall* PFN_AIL_set_digital_master_reverb)(HDIGDRIVER dig, float reverb_decay_time,
                                                            float reverb_predelay, float reverb_damping);
typedef void (__stdcall* PFN_AIL_stop_sample)(HSAMPLE sample);
typedef void (__stdcall* PFN_AIL_set_sample_playback_rate)(HSAMPLE sample, int32_t playback_rate);
typedef void (__stdcall* PFN_AIL_release_sample_handle)(HSAMPLE sample);
typedef void (__stdcall* PFN_AIL_decompress_ADPCM)(void* data, void** outdata, uint32_t* outsize);
typedef int32_t (__stdcall* PFN_AIL_digital_CPU_percent)(HDIGDRIVER dig);
typedef void*(__stdcall* PFN_AIL_speaker_configuration)(HDIGDRIVER dig, int32_t* phys_chans, int32_t* channels,
                                                        float* falloff_power, MSS_MC_SPEC* spec);
typedef uint32_t (__stdcall* PFN_AIL_set_speaker_configuration)(HDIGDRIVER dig, void* data, int32_t channels,
                                                                float falloff_power);
typedef void (__stdcall* PFN_AIL_mem_free_lock)(void* ptr);
typedef void (__stdcall* PFN_AIL_digital_handle_release)(HDIGDRIVER dig);
typedef void (__stdcall* PFN_AIL_shutdown)();
typedef void (__stdcall* PFN_AIL_WAV_info)(void*, void* info);
typedef void (__stdcall* PFN_AIL_decompress_ASI)(void*, uint32_t, const char*, void*, uint32_t*, void*);
typedef int (__stdcall* PFN_AIL_file_type)(const void*, uint32_t size);
typedef void* (__stdcall* PFN_AIL_file_read)(const char* filename, void* dest);
typedef uint32_t (__stdcall* PFN_AIL_file_size)(const char* filename);
typedef void (__stdcall* PFN_AIL_init_sample)(HSAMPLE, int);
typedef HSAMPLE (__stdcall* PFN_AIL_allocate_sample_handle)(HDIGDRIVER dig);
typedef void (__stdcall *PFN_AIL_set_stream_position)(HSTREAM stream, int32_t offset);
typedef uint32_t(__stdcall* PFN_AIL_sample_playback_rate)(HSAMPLE sample);
typedef uint32_t(__stdcall* PFN_AIL_sample_status)(HSAMPLE sample);
typedef uint32_t(__stdcall* PFN_AIL_stream_status)(HSTREAM stream);
typedef uint32_t(__stdcall* PFN_AIL_sample_position)(HSAMPLE sample);
typedef void(__stdcall* PFN_AIL_stream_ms_position)(HSAMPLE sample, int32_t *total, int32_t *current);
typedef void(__stdcall* PFN_AIL_set_sample_loop_count)(HSAMPLE sample, int32_t loop);
typedef HSTREAM(__stdcall* PFN_AIL_open_stream)(HDIGDRIVER dig, const char* filename, int32_t memory);
typedef void(__stdcall* PFN_AIL_set_stream_user_data)(HSTREAM stream, uint32_t index, int32_t value);
typedef void(__stdcall* PFN_AIL_close_digital_driver)(HDIGDRIVER dig);


struct AIL {
	AIL();
	~AIL();

	AIL(const AIL&) = delete;
	AIL(AIL&&) = delete;
	AIL& operator=(const AIL&) = delete;
	AIL& operator=(AIL&&) = delete;
	
	HMODULE mss32;
	PFN_AIL_last_error last_error;
	PFN_AIL_set_redist_directory set_redist_directory;
	PFN_AIL_set_file_callbacks set_file_callbacks;
	PFN_AIL_startup startup;
	PFN_AIL_open_digital_driver open_digital_driver;
	PFN_AIL_set_DirectSound_HWND set_DirectSound_HWND;
	PFN_AIL_close_stream close_stream;
	PFN_AIL_pause_stream pause_stream;
	PFN_AIL_stream_position stream_position;
	PFN_AIL_set_stream_loop_count set_stream_loop_count;
	PFN_AIL_stream_sample_handle stream_sample_handle;
	PFN_AIL_stream_info stream_info;
	PFN_AIL_set_sample_volume_pan set_sample_volume_pan;
	PFN_AIL_start_stream start_stream;
	PFN_AIL_set_sample_file set_sample_file;
	PFN_AIL_set_sample_3D_distances set_sample_3D_distances;
	PFN_AIL_set_sample_3D_position set_sample_3D_position;
	PFN_AIL_start_sample start_sample;
	PFN_AIL_set_named_sample_file set_named_sample_file;
	PFN_AIL_set_digital_master_reverb set_digital_master_reverb;
	PFN_AIL_stop_sample stop_sample;
	PFN_AIL_set_sample_playback_rate set_sample_playback_rate;
	PFN_AIL_release_sample_handle release_sample_handle;
	PFN_AIL_decompress_ADPCM decompress_ADPCM;
	PFN_AIL_digital_CPU_percent digital_CPU_percent;
	PFN_AIL_speaker_configuration speaker_configuration;
	PFN_AIL_set_speaker_configuration set_speaker_configuration;
	PFN_AIL_mem_free_lock mem_free_lock;
	PFN_AIL_digital_handle_release digital_handle_release;
	PFN_AIL_shutdown shutdown;
	PFN_AIL_WAV_info WAV_info;
	PFN_AIL_decompress_ASI decompress_ASI;
	PFN_AIL_file_type file_type;
	PFN_AIL_file_read file_read;
	PFN_AIL_file_size file_size;
	PFN_AIL_init_sample init_sample;
	PFN_AIL_allocate_sample_handle allocate_sample_handle;
	PFN_AIL_set_stream_position set_stream_position;
	PFN_AIL_sample_playback_rate sample_playback_rate;
	PFN_AIL_sample_status sample_status;
	PFN_AIL_stream_status stream_status;
	PFN_AIL_sample_position sample_position;
	PFN_AIL_stream_ms_position stream_ms_position;
	PFN_AIL_set_sample_loop_count set_sample_loop_count;
	PFN_AIL_open_stream open_stream;
	PFN_AIL_set_stream_user_data set_stream_user_data;
	PFN_AIL_close_digital_driver close_digital_driver;
};

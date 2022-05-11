#pragma once

typedef enum class UAUDIO_RESULT
{
	UAUDIO_OK,

	UAUDIO_ERR_FILE_NOTFOUND,
	UAUDIO_ERR_FILE_BAD,

	UAUDIO_ERR_NO_FMT_CHUNK,
	UAUDIO_ERR_NO_DATA_CHUNK,
	UAUDIO_ERR_CHUNK_NOT_FOUND,

	UAUDIO_ERR_NO_CHANNEL,
	UAUDIO_ERR_SOUND_NOT_SET,
	UAUDIO_ERR_CHANNEL_NOT_PLAYING,

	UAUDIO_ERR_NOT_ENOUGH_BUFFER_SPACE,

	UAUDIO_ERR_NO_BACKEND,

	UAUDIO_ERR_SOUND_NOT_FOUND,

	UAUDIO_ERR_NUM_CHANNELS_INVALID,
	UAUDIO_ERR_BITS_PER_SAMPLE_INVALID,

	UAUDIO_ERR_XAUDIO2_NO_SOURCEVOICE,
	UAUDIO_ERR_XAUDIO2_SOURCEVOICE_CREATION_FAILED,
	UAUDIO_ERR_XAUDIO2_NO_SYSTEM,
} UAUDIO_RESULT;
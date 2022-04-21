#pragma once

namespace uaudio
{
	/*
	 * You can change the default allocator of UAUDIO. The allocator must have an allocate and deallocate method.
	 * You could inherit from std::allocator and overload the methods there to ensure it works.
	 */
	// #define UAUDIO_DEFAULT_ALLOCATOR std::allocator

	/*
	 * You can change the free and malloc methods which get used in the engine.
	 */
	// #define UAUDIO_DEFAULT_ALLOC malloc
	// #define UAUDIO_DEFAULT_FREE free

	/*
	 * If for whatever reason you want the volume to be lower on start-up or the panning to be different, you can override these.
	 */
	// #define UAUDIO_DEFAULT_VOLUME 1.0f
	// #define UAUDIO_DEFAULT_PANNING 0.0f
	/*
	 * A higher or lower buffer size is also possible. You can change this here.
	 */
	// #define UAUDIO_DEFAULT_BUFFERSIZE uaudio::BUFFERSIZE::BUFFERSIZE_8192

	/*
	 * If you want to change the hash class and hashing method, you can override them here.
	 */
	// #define UAUDIO_DEFAULT_HASH Hash
	// #define UAUDIO_DEFAULT_HASH_FUNCTION GetHash

	/*
	 * You can disable the colors in the logger if you do not want them.
	 */
	// #define UAUDIO_COLOR_LOGGING true
	// #define MAX_BUFFER_LOGGER 256

	/*
	 * You can change the default loading config here:
	 */
	// #define UAUDIO_DEFAULT_CHUNKS "fmt ", "data"
	// #define UAUDIO_DEFAULT_CHANNELS 2
	// #define UAUDIO_DEFAULT_BITS_PER_SAMPLE 16

	/*
	 * The default number of channels for the audio system.
	 */
	// #define UAUDIO_DEFAULT_NUM_CHANNELS 20
}
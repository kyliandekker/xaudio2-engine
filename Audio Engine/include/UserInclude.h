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
//#define UAUDIO_DEFAULT_ALLOC malloc
//#define UAUDIO_DEFAULT_FREE free

/*
 * If for whatever reason you want the volume to be lower on start-up or the panning to be different, you can override these.
 */
//constexpr float UAUDIO_DEFAULT_VOLUME = 1.0f;
//constexpr float UAUDIO_DEFAULT_PANNING = 0.0f;

/*
 * A higher or lower buffer size is also possible. You can change this here.
 */
//constexpr BUFFERSIZE UAUDIO_DEFAULT_BUFFERSIZE = BUFFERSIZE::BUFFERSIZE_8192;

/*
 * If you want to change the hash class and hashing method, you can override them here.
 */
//#define UAUDIO_DEFAULT_HASH Hash
//#define UAUDIO_DEFAULT_HASH_FUNCTION GetHash

/*
 * You can disable the colors in the logger if you do not want them.
 */
//#define UAUDIO_NO_COLOR_LOGGING true
}
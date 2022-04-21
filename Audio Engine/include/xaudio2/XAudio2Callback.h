#pragma once

#include <xaudio2.h>

namespace uaudio::xaudio2
{
	struct XAudio2Callback : IXAudio2VoiceCallback
	{
		XAudio2Callback() : IXAudio2VoiceCallback() {}
		XAudio2Callback(const XAudio2Callback& rhs) = default;
		~XAudio2Callback() = default;

		XAudio2Callback& operator=(const XAudio2Callback& rhs) = default;

		STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32) override
		{}
		STDMETHOD_(void, OnVoiceProcessingPassEnd)() override
		{}
		STDMETHOD_(void, OnStreamEnd)() override
		{}
		STDMETHOD_(void, OnBufferStart)(void*) override
		{}
		STDMETHOD_(void, OnBufferEnd)(void*) override
		{}
		STDMETHOD_(void, OnLoopEnd)(void*) override
		{}
		STDMETHOD_(void, OnVoiceError)(void*, HRESULT) override
		{}
	};
}

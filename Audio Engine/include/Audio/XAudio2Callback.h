#pragma once

#include <xaudio2.h>

struct XAudio2Callback : public IXAudio2VoiceCallback
{
	virtual ~XAudio2Callback() = default;

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
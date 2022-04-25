#pragma once

struct UserAudioCallback
{
	UserAudioCallback() = default;
	UserAudioCallback(const UserAudioCallback& rhs) = default;
	virtual ~UserAudioCallback() = default;

	UserAudioCallback& operator=(const UserAudioCallback& rhs) = default;

	virtual void OnEnd() { }
	virtual void OnPause() { }
	virtual void OnStart() { }
	virtual void OnStop() { }
};
# XAudio2 Engine

This is a personal project of mine which I have been working on during my second year at Breda University of Applied Sciences. It uses XAudio2 and has its own wav file converter.

## Usage

The project comes with its own debugging window (SDL is used), but it can also be completely ignored. Using it in something like a game engine is very easy, as you would only have to include the AudioSystem class.

```cpp
#include "Audio/AudioSystem.h"

void main()
{
	AudioSystem audioSystem;
	while (true)
	{
		audioSystem.Update();
	}
}
```

Creating sounds and playing them is also very easy.

```cpp
#include "Audio/AudioSystem.h"

void main()
{
	AudioSystem audioSystem;
	int soundHandle = audioSystem.CreateSound("./resources/audio/player_hurt.wav"); 

	int channel = audioSystem.Play(soundHandle); 

	audioSystem.Stop(soundHandle); 
}
```
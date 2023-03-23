#include "audioemitter.h"

namespace Saga {
	AudioEmitter::AudioEmitter() : preload(false), playOnAwake(false) { }
	AudioEmitter::AudioEmitter(std::string eventName, bool playOnAwake, bool preload) 
	: eventName(eventName), playOnAwake(playOnAwake), preload(preload) { }
};
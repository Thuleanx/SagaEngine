#include "audioemitter.h"

namespace Saga {
	AudioEmitter::AudioEmitter() { }
	AudioEmitter::AudioEmitter(std::string eventName, bool playOnAwake, bool preload) 
	: eventName(eventName), playOnAwake(playOnAwake), preload(preload) { }
};
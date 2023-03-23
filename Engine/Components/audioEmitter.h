#pragma once

#include <string>
#include "../Audio/audioEngine.h"

namespace Saga {
struct AudioEmitter {
	std::string eventName;
	bool playOnAwake;
	bool preload;

	// these are values you won't need to set
	AudioEventInstance audioInstance;
	bool is3D;

	AudioEmitter();
	AudioEmitter(std::string eventName, bool playOnAwake = false, bool preload = false);
};
}; // namespace Saga

#pragma once

#include "fmod.h"
#include "fmod_studio.h"
#include <memory>

namespace Saga {

class AudioEventInstance : std::enable_shared_from_this<AudioEventInstance> {
public:
	AudioEventInstance(FMOD_STUDIO_EVENTINSTANCE* instance);
	~AudioEventInstance();

	FMOD_STUDIO_EVENTINSTANCE* getInstance();
private:
	FMOD_STUDIO_EVENTINSTANCE* instance;
};

} // namespace Saga

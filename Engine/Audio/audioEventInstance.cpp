#include "audioEventInstance.h"
#include "audioEngine.h"
#include "../_Core/logger.h"

namespace Saga {

AudioEventInstance::AudioEventInstance(FMOD_STUDIO_EVENTINSTANCE *instance) : instance(instance) { }
AudioEventInstance::~AudioEventInstance() {}

FMOD_STUDIO_EVENTINSTANCE* AudioEventInstance::getInstance() {
	return instance;
}

} // namespace Saga

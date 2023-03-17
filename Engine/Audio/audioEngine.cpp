#include "audioEngine.h"
#include "Engine/_Core/logger.h"
#include <cstdarg>

namespace Saga::AudioEngine {
	AudioImplementation implementation = AudioImplementation();

	namespace {
        void ensureFMOD_OK(FMOD_RESULT result, const char* failMessage, ...) {
			if (result != FMOD_RESULT::FMOD_OK) {
				va_list arg_ptr;
				va_start(arg_ptr, failMessage);
                SERROR(("FMOD Error: %s. With mesage: " + std::string(failMessage)).c_str(), result, arg_ptr);
				va_end(arg_ptr);
			}
		}
	}

	bool init() {
		ensureFMOD_OK(FMOD_Studio_System_Create(&implementation.studioSystem, FMOD_VERSION), "Failed to create fmod studio system.");
		SINFO("Fmod Studio System Created.");
		ensureFMOD_OK(FMOD_Studio_System_Initialize(implementation.studioSystem, maxChannels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr), "Failed to initialize fmod studio system.");
		SINFO("Fmod Studio System Initialized.");
		return true;
	}

	void update() {
		ensureFMOD_OK(FMOD_Studio_System_Update(implementation.studioSystem), "Failed to update fmod studio system.");
	}

	bool release() {
		ensureFMOD_OK(FMOD_Studio_System_Release(implementation.studioSystem), "Failed to release fmod system.");
        SINFO("Fmod Studio System Released.");
		return true;
	}

	void loadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
		if (implementation.banks.count(bankName)) {
			SWARN("Tried to load already loaded fmod bank %s. Does nothing.", bankName.c_str());
			return;
		}
		FMOD_STUDIO_BANK* bank;
		ensureFMOD_OK(FMOD_Studio_System_LoadBankFile(implementation.studioSystem, bankName.c_str(), flags, &bank), "Can't load fmod bank \"%s\". Ensure the provided name is correct.", bankName.c_str());
		if (bank) implementation.banks[bankName] = bank;
	}

	void unloadBank(const std::string& bankName) {
		if (!implementation.banks.count(bankName)) {
			SWARN("Tried to unload not loaded fmod bank %s. Please ensure correct bank name.", bankName.c_str());
			return;
		}
		ensureFMOD_OK(FMOD_Studio_Bank_Unload(implementation.banks[bankName]), "Can't unload fmod bank \"%s\". Ensure the provided name is correct.", bankName.c_str());
		implementation.banks.erase(bankName);
	}

	void loadEvent(const std::string& eventName) {
		if (implementation.events.count(eventName)) {
			SWARN("Tried to load already loaded fmod event %s. Please ensure the correct event name.", eventName.c_str());
			return;
		}
		FMOD_STUDIO_EVENTDESCRIPTION *description = NULL;
		ensureFMOD_OK(FMOD_Studio_System_GetEvent(implementation.studioSystem, eventName.c_str(), &description), "Failed to load event: %s.", eventName.c_str());
		implementation.events[eventName] = description;
	}

	FMOD_STUDIO_EVENTINSTANCE* playEvent(const std::string& eventName) {
		if (!implementation.events.count(eventName)) {
			SWARN("Event sample data for fmod event \"%s\" has not been loaded. Please load the event first!", eventName.c_str());
            return NULL;
		}
		FMOD_STUDIO_EVENTINSTANCE *instance = NULL;
		playEvent(instance);
		return instance;
	}

	void playEvent(FMOD_STUDIO_EVENTINSTANCE* instance) {
		ensureFMOD_OK(FMOD_Studio_EventInstance_Start(instance), "Failed to start fmod event.");
	}

    void stopEvent(FMOD_STUDIO_EVENTINSTANCE* event, bool immediate) {
		if (!event) {
			SERROR("Tried to stop a null event.");
			return;
		}

		FMOD_STUDIO_STOP_MODE mode = immediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
		ensureFMOD_OK(FMOD_Studio_EventInstance_Stop(event, mode), "Cannot stop an fmod event.");
	}

	void releaseEvent(FMOD_STUDIO_EVENTINSTANCE* event) {
		if (!event) {
			SERROR("Tried to release a null event.");
			return;
		}
		ensureFMOD_OK(FMOD_Studio_EventInstance_Release(event), "Cannot release an fmod event.");
	}

	void releaseAllEventInstances(const std::string& eventName) {
		if (!implementation.events.count(eventName)) {
			SWARN("Tried to release instances of event %s that has not been loaded.", eventName.c_str());
			return;
		}
		ensureFMOD_OK(FMOD_Studio_EventDescription_ReleaseAllInstances(implementation.events[eventName]), "Fail to release instance for event \"%s\".", eventName.c_str());
	}

	void unloadEvent(const std::string& eventName) {
		if (!implementation.events.count(eventName)) {
			SWARN("Tried to unload event %s that has not been loaded.", eventName.c_str());
			return;
		}
		// this doesn't neccessarily unload until all instances of the event is released, so beware
		ensureFMOD_OK(FMOD_Studio_EventDescription_UnloadSampleData(implementation.events[eventName]), "Fail to unload sample data for event \"%s\".", eventName.c_str());
		implementation.events.erase(eventName);
	}


	void loadsound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream) { }
	void unLoadSound(const std::string& strSoundName) { }
	void set3dListenerAndOrientation(const glm::vec3& vPos, float fVolumedB) { }
	void playSound(const std::string& strSoundName, const glm::vec3& vPos, float fVolumedB) { }
	void stopChannel(int nChannelId) { }
	void stopEvent(const std::string& eventName, bool bImmediate) { }
	void geteventParameter(const std::string& eventName, const std::string& strEventParameter, float* parameter) { }
	void setEventParameter(const std::string& eventName, const std::string& strParameterName, float fValue) { }
	void stopAllChannels() { }
	void setChannel3dPosition(int nChannelId, const glm::vec3& vPosition) { }
	void setChannelvolume(int nChannelId, float fVolumedB) { }
    bool isPlaying(int nChannelId) { return false; }
    bool isEventPlaying(const std::string& eventName) { return false; }
    float dbToVolume(float db) { return 0; }
    float volumeTodb(float volume) { return 0; }
    FMOD_VECTOR vectorToFmod(const glm::vec3& vPosition) { return FMOD_VECTOR{vPosition.x, vPosition.y, vPosition.z}; }
}

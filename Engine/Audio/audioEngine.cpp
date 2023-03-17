#include "audioEngine.h"
#include "Engine/_Core/logger.h"
#include <cstdarg>

namespace Saga::AudioEngine {
	AudioImplementation implementation = AudioImplementation();

	namespace {
		void ensureFMOD_OK(FMOD_RESULT result, const std::string& failMessage, ...) {
			if (result != FMOD_RESULT::FMOD_OK) {
				va_list arg_ptr;
				va_start(arg_ptr, failMessage);
				SERROR(("FMOD Error: %s. With mesage: " + failMessage).c_str(), result, arg_ptr);
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
		delete implementation.studioSystem;
		return true;
	}

	void loadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
		if (implementation.banks.count(strBankName)) {
			SWARN("Tried to load already loaded fmod bank %s. Does nothing.", strBankName.c_str());
			return;
		}
		FMOD_STUDIO_BANK* bank;
		ensureFMOD_OK(FMOD_Studio_System_LoadBankFile(implementation.studioSystem, strBankName.c_str(), flags, &bank), "Can't load fmod bank \"%s\". Ensure the provided name is correct.", strBankName.c_str());
		if (bank) implementation.banks[strBankName] = bank;
	}

	void unloadBank(const std::string& strBankName) {
		if (!implementation.banks.count(strBankName)) {
			SWARN("Tried to unload not loaded fmod bank %s. Please ensure correct bank name.", strBankName.c_str());
			return;
		}
		ensureFMOD_OK(FMOD_Studio_Bank_Unload(implementation.banks[strBankName]), "Can't unload fmod bank \"%s\". Ensure the provided name is correct.", strBankName.c_str());
		implementation.banks.erase(strBankName);
	}

	void loadEvent(const std::string& strEventName) { }
	void loadsound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream) { }
	void unLoadSound(const std::string& strSoundName) { }
	void set3dListenerAndOrientation(const glm::vec3& vPos, float fVolumedB) { }
	void playSound(const std::string& strSoundName, const glm::vec3& vPos, float fVolumedB) { }
	void playEvent(const std::string& strEventName) { }
	void stopChannel(int nChannelId) { }
	void stopEvent(const std::string& strEventName, bool bImmediate) { }
	void geteventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter) { }
	void setEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue) { }
	void stopAllChannels() { }
	void setChannel3dPosition(int nChannelId, const glm::vec3& vPosition) { }
	void setChannelvolume(int nChannelId, float fVolumedB) { }
    bool isPlaying(int nChannelId) { return false; }
    bool isEventPlaying(const std::string& strEventName) { return false; }
    float dbToVolume(float db) { return 0; }
    float volumeTodb(float volume) { return 0; }
    FMOD_VECTOR vectorToFmod(const glm::vec3& vPosition) { return FMOD_VECTOR{vPosition.x, vPosition.y, vPosition.z}; }
}

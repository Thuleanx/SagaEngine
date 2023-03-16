#include "audioEngine.h"
#include "Engine/_Core/logger.h"

namespace Saga::AudioEngine {
	AudioImplementation implementation = AudioImplementation();

	bool init() {
		FMOD_Studio_System_Create(&implementation.studioSystem, FMOD_VERSION);
		SINFO("Fmod Studio System Created.");
		FMOD_Studio_System_Initialize(implementation.studioSystem, maxChannels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
		SINFO("Fmod Studio System Initialized.");
		return true;
	}
	void update() {

	}

	bool release() {
		FMOD_Studio_System_Release(implementation.studioSystem);
		SINFO("Fmod Studio System Released.");
		return true;
	}

	void loadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) { }
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

#include "audioEngine.h"
#include "Engine/_Core/logger.h"
#include <cstdarg>
#include <iostream>

#define str(x) #x
#define xstr(x) str(x)

namespace Saga::AudioEngine {
	AudioImplementation implementation = AudioImplementation();

	namespace {
        void ensureFMOD_OK(FMOD_RESULT result, const char* failMessage, ...) {
			if (result != FMOD_RESULT::FMOD_OK) {
				va_list arg_ptr;
				va_start(arg_ptr, failMessage);
				char outMessage[32000] = {0};
				vsnprintf(outMessage, 32000, failMessage, arg_ptr);
				va_end(arg_ptr);

				std::string fmodResultStringified = "Unknown Error";
				switch (result) {
					case FMOD_OK : fmodResultStringified = "FMOD_OK"; break;
					case FMOD_ERR_BADCOMMAND : fmodResultStringified = "FMOD_ERR_BADCOMMAND"; break;
					case FMOD_ERR_CHANNEL_ALLOC : fmodResultStringified = "FMOD_ERR_CHANNEL_ALLOC"; break;
					case FMOD_ERR_CHANNEL_STOLEN : fmodResultStringified = "FMOD_ERR_CHANNEL_STOLEN"; break;
					case FMOD_ERR_DMA : fmodResultStringified = "FMOD_ERR_DMA"; break;
					case FMOD_ERR_DSP_CONNECTION : fmodResultStringified = "FMOD_ERR_DSP_CONNECTION"; break;
					case FMOD_ERR_DSP_DONTPROCESS : fmodResultStringified = "FMOD_ERR_DSP_DONTPROCESS"; break;
					case FMOD_ERR_DSP_FORMAT : fmodResultStringified = "FMOD_ERR_DSP_FORMAT"; break;
					case FMOD_ERR_DSP_INUSE : fmodResultStringified = "FMOD_ERR_DSP_INUSE"; break;
					case FMOD_ERR_DSP_NOTFOUND : fmodResultStringified = "FMOD_ERR_DSP_NOTFOUND"; break;
					case FMOD_ERR_DSP_RESERVED : fmodResultStringified = "FMOD_ERR_DSP_RESERVED"; break;
					case FMOD_ERR_DSP_SILENCE : fmodResultStringified = "FMOD_ERR_DSP_SILENCE"; break;
					case FMOD_ERR_DSP_TYPE : fmodResultStringified = "FMOD_ERR_DSP_TYPE"; break;
					case FMOD_ERR_FILE_BAD : fmodResultStringified = "FMOD_ERR_FILE_BAD"; break;
					case FMOD_ERR_FILE_COULDNOTSEEK : fmodResultStringified = "FMOD_ERR_FILE_COULDNOTSEEK"; break;
					case FMOD_ERR_FILE_DISKEJECTED : fmodResultStringified = "FMOD_ERR_FILE_DISKEJECTED"; break;
					case FMOD_ERR_FILE_EOF : fmodResultStringified = "FMOD_ERR_FILE_EOF"; break;
					case FMOD_ERR_FILE_ENDOFDATA : fmodResultStringified = "FMOD_ERR_FILE_ENDOFDATA"; break;
					case FMOD_ERR_FILE_NOTFOUND : fmodResultStringified = "FMOD_ERR_FILE_NOTFOUND"; break;
					case FMOD_ERR_FORMAT : fmodResultStringified = "FMOD_ERR_FORMAT"; break;
					case FMOD_ERR_HEADER_MISMATCH : fmodResultStringified = "FMOD_ERR_HEADER_MISMATCH"; break;
					case FMOD_ERR_HTTP : fmodResultStringified = "FMOD_ERR_HTTP"; break;
					case FMOD_ERR_HTTP_ACCESS : fmodResultStringified = "FMOD_ERR_HTTP_ACCESS"; break;
					case FMOD_ERR_HTTP_PROXY_AUTH : fmodResultStringified = "FMOD_ERR_HTTP_PROXY_AUTH"; break;
					case FMOD_ERR_HTTP_SERVER_ERROR : fmodResultStringified = "FMOD_ERR_HTTP_SERVER_ERROR"; break;
					case FMOD_ERR_HTTP_TIMEOUT : fmodResultStringified = "FMOD_ERR_HTTP_TIMEOUT"; break;
					case FMOD_ERR_INITIALIZATION : fmodResultStringified = "FMOD_ERR_INITIALIZATION"; break;
					case FMOD_ERR_INITIALIZED : fmodResultStringified = "FMOD_ERR_INITIALIZED"; break;
					case FMOD_ERR_INTERNAL : fmodResultStringified = "FMOD_ERR_INTERNAL"; break;
					case FMOD_ERR_INVALID_FLOAT : fmodResultStringified = "FMOD_ERR_INVALID_FLOAT"; break;
					case FMOD_ERR_INVALID_HANDLE : fmodResultStringified = "FMOD_ERR_INVALID_HANDLE"; break;
					case FMOD_ERR_INVALID_PARAM : fmodResultStringified = "FMOD_ERR_INVALID_PARAM"; break;
					case FMOD_ERR_INVALID_POSITION : fmodResultStringified = "FMOD_ERR_INVALID_POSITION"; break;
					case FMOD_ERR_INVALID_SPEAKER : fmodResultStringified = "FMOD_ERR_INVALID_SPEAKER"; break;
					case FMOD_ERR_INVALID_SYNCPOINT : fmodResultStringified = "FMOD_ERR_INVALID_SYNCPOINT"; break;
					case FMOD_ERR_INVALID_THREAD : fmodResultStringified = "FMOD_ERR_INVALID_THREAD"; break;
					case FMOD_ERR_INVALID_VECTOR : fmodResultStringified = "FMOD_ERR_INVALID_VECTOR"; break;
					case FMOD_ERR_MAXAUDIBLE : fmodResultStringified = "FMOD_ERR_MAXAUDIBLE"; break;
					case FMOD_ERR_MEMORY : fmodResultStringified = "FMOD_ERR_MEMORY"; break;
					case FMOD_ERR_MEMORY_CANTPOINT : fmodResultStringified = "FMOD_ERR_MEMORY_CANTPOINT"; break;
					case FMOD_ERR_NEEDS3D : fmodResultStringified = "FMOD_ERR_NEEDS3D"; break;
					case FMOD_ERR_NEEDSHARDWARE : fmodResultStringified = "FMOD_ERR_NEEDSHARDWARE"; break;
					case FMOD_ERR_NET_CONNECT : fmodResultStringified = "FMOD_ERR_NET_CONNECT"; break;
					case FMOD_ERR_NET_SOCKET_ERROR : fmodResultStringified = "FMOD_ERR_NET_SOCKET_ERROR"; break;
					case FMOD_ERR_NET_URL : fmodResultStringified = "FMOD_ERR_NET_URL"; break;
					case FMOD_ERR_NET_WOULD_BLOCK : fmodResultStringified = "FMOD_ERR_NET_WOULD_BLOCK"; break;
					case FMOD_ERR_NOTREADY : fmodResultStringified = "FMOD_ERR_NOTREADY"; break;
					case FMOD_ERR_OUTPUT_ALLOCATED : fmodResultStringified = "FMOD_ERR_OUTPUT_ALLOCATED"; break;
					case FMOD_ERR_OUTPUT_CREATEBUFFER : fmodResultStringified = "FMOD_ERR_OUTPUT_CREATEBUFFER"; break;
					case FMOD_ERR_OUTPUT_DRIVERCALL : fmodResultStringified = "FMOD_ERR_OUTPUT_DRIVERCALL"; break;
					case FMOD_ERR_OUTPUT_FORMAT : fmodResultStringified = "FMOD_ERR_OUTPUT_FORMAT"; break;
					case FMOD_ERR_OUTPUT_INIT : fmodResultStringified = "FMOD_ERR_OUTPUT_INIT"; break;
					case FMOD_ERR_OUTPUT_NODRIVERS : fmodResultStringified = "FMOD_ERR_OUTPUT_NODRIVERS"; break;
					case FMOD_ERR_PLUGIN : fmodResultStringified = "FMOD_ERR_PLUGIN"; break;
					case FMOD_ERR_PLUGIN_MISSING : fmodResultStringified = "FMOD_ERR_PLUGIN_MISSING"; break;
					case FMOD_ERR_PLUGIN_RESOURCE : fmodResultStringified = "FMOD_ERR_PLUGIN_RESOURCE"; break;
					case FMOD_ERR_PLUGIN_VERSION : fmodResultStringified = "FMOD_ERR_PLUGIN_VERSION"; break;
					case FMOD_ERR_RECORD : fmodResultStringified = "FMOD_ERR_RECORD"; break;
					case FMOD_ERR_REVERB_CHANNELGROUP : fmodResultStringified = "FMOD_ERR_REVERB_CHANNELGROUP"; break;
					case FMOD_ERR_REVERB_INSTANCE : fmodResultStringified = "FMOD_ERR_REVERB_INSTANCE"; break;
					case FMOD_ERR_SUBSOUNDS : fmodResultStringified = "FMOD_ERR_SUBSOUNDS"; break;
					case FMOD_ERR_SUBSOUND_ALLOCATED : fmodResultStringified = "FMOD_ERR_SUBSOUND_ALLOCATED"; break;
					case FMOD_ERR_SUBSOUND_CANTMOVE : fmodResultStringified = "FMOD_ERR_SUBSOUND_CANTMOVE"; break;
					case FMOD_ERR_TAGNOTFOUND : fmodResultStringified = "FMOD_ERR_TAGNOTFOUND"; break;
					case FMOD_ERR_TOOMANYCHANNELS : fmodResultStringified = "FMOD_ERR_TOOMANYCHANNELS"; break;
					case FMOD_ERR_TRUNCATED : fmodResultStringified = "FMOD_ERR_TRUNCATED"; break;
					case FMOD_ERR_UNIMPLEMENTED : fmodResultStringified = "FMOD_ERR_UNIMPLEMENTED"; break;
					case FMOD_ERR_UNINITIALIZED : fmodResultStringified = "FMOD_ERR_UNINITIALIZED"; break;
					case FMOD_ERR_UNSUPPORTED : fmodResultStringified = "FMOD_ERR_UNSUPPORTED"; break;
					case FMOD_ERR_VERSION : fmodResultStringified = "FMOD_ERR_VERSION"; break;
					case FMOD_ERR_EVENT_ALREADY_LOADED : fmodResultStringified = "FMOD_ERR_EVENT_ALREADY_LOADED"; break;
					case FMOD_ERR_EVENT_LIVEUPDATE_BUSY : fmodResultStringified = "FMOD_ERR_EVENT_LIVEUPDATE_BUSY"; break;
					case FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH : fmodResultStringified = "FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH"; break;
					case FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT : fmodResultStringified = "FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT"; break;
					case FMOD_ERR_EVENT_NOTFOUND : fmodResultStringified = "FMOD_ERR_EVENT_NOTFOUND"; break;
					case FMOD_ERR_STUDIO_UNINITIALIZED : fmodResultStringified = "FMOD_ERR_STUDIO_UNINITIALIZED"; break;
					case FMOD_ERR_STUDIO_NOT_LOADED : fmodResultStringified = "FMOD_ERR_STUDIO_NOT_LOADED"; break;
					case FMOD_ERR_INVALID_STRING : fmodResultStringified = "FMOD_ERR_INVALID_STRING"; break;
					case FMOD_ERR_ALREADY_LOCKED : fmodResultStringified = "FMOD_ERR_ALREADY_LOCKED"; break;
					case FMOD_ERR_NOT_LOCKED : fmodResultStringified = "FMOD_ERR_NOT_LOCKED"; break;
					case FMOD_ERR_RECORD_DISCONNECTED : fmodResultStringified = "FMOD_ERR_RECORD_DISCONNECTED"; break;
					case FMOD_ERR_TOOMANYSAMPLES : fmodResultStringified = "FMOD_ERR_TOOMANYSAMPLES"; break;
					case FMOD_RESULT_FORCEINT : fmodResultStringified = "FMOD_RESULT_FORCEINT"; break;
				}

				SERROR(("FMOD Error: %s. With mesage: " + std::string(outMessage)).c_str(), fmodResultStringified.c_str());
			}
		}
	}

	bool init() {
		ensureFMOD_OK(FMOD_Studio_System_Create(&implementation.studioSystem, FMOD_VERSION), "Failed to create fmod studio system.");
		SINFO("Fmod Studio System Created.");
        ensureFMOD_OK(FMOD_Studio_System_Initialize(implementation.studioSystem, maxChannels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0), "Failed to initialize fmod studio system.");
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
		SINFO("Attempting to load bank: %s.", bankName.c_str());
		if (implementation.banks.count(bankName)) {
			SWARN("Tried to load already loaded fmod bank %s. Does nothing.", bankName.c_str());
			return;
		}
		FMOD_STUDIO_BANK* bank;
		FMOD_RESULT status = FMOD_Studio_System_LoadBankFile(implementation.studioSystem, bankName.c_str(), flags, &bank);
		ensureFMOD_OK(status, "Can't load fmod bank \"%s\". Ensure the provided name is correct.", bankName.c_str());
		SINFO("Loading bank success: %s.", bankName.c_str());
		if (bank) implementation.banks[bankName] = bank;
	}

	void unloadBank(const std::string& bankName) {
		SINFO("Attempting to unload bank: %s.", bankName.c_str());
		if (!implementation.banks.count(bankName)) {
			SWARN("Tried to unload not loaded fmod bank %s. Please ensure correct bank name.", bankName.c_str());
			return;
		}
		ensureFMOD_OK(FMOD_Studio_Bank_Unload(implementation.banks[bankName]), "Can't unload fmod bank \"%s\". Ensure the provided name is correct.", bankName.c_str());
		SINFO("Unload bank success: %s.", bankName.c_str());
		implementation.banks.erase(bankName);
	}

	void loadEvent(const std::string& eventName) {
		if (implementation.events.count(eventName)) {
			SWARN("Tried to load already loaded fmod event %s. Please ensure the correct event name.", eventName.c_str());
			return;
		}
		FMOD_STUDIO_EVENTDESCRIPTION *description = NULL;
		ensureFMOD_OK(FMOD_Studio_System_GetEvent(implementation.studioSystem, eventName.c_str(), &description), 
			"Failed to load event: %s.", eventName.c_str());
		implementation.events[eventName] = description;
	}

	std::shared_ptr<AudioEventInstance> playEvent(const std::string& eventName) {
		if (!implementation.events.count(eventName)) {
			SWARN("Event sample data for fmod event \"%s\" has not been loaded. Please load the event first!", eventName.c_str());
            return NULL;
		}
		FMOD_STUDIO_EVENTINSTANCE *instance = NULL;
		ensureFMOD_OK(FMOD_Studio_EventDescription_CreateInstance(implementation.events[eventName], &instance), 
			"Failed to create event instance for event %s.", eventName.c_str());

		std::shared_ptr<AudioEventInstance> sharedInstance = std::make_shared<AudioEventInstance>(instance);
        playEvent(sharedInstance);
		std::cout << sharedInstance->getInstance() << std::endl;
		return sharedInstance;
	}

	void playEvent(std::shared_ptr<AudioEventInstance> instance) {
		ensureFMOD_OK(FMOD_Studio_EventInstance_Start(instance->getInstance()), "Failed to start fmod event.");
	}

    void stopEvent(std::shared_ptr<AudioEventInstance> event, bool immediate) {
		if (!event) {
			SERROR("Tried to stop a null event.");
			return;
		}
		std::cout << event->getInstance() << std::endl;
		FMOD_STUDIO_STOP_MODE mode = immediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
		ensureFMOD_OK(FMOD_Studio_EventInstance_Stop(event->getInstance(), mode), "Cannot stop an fmod event.");
	}

	void releaseEvent(std::shared_ptr<AudioEventInstance> event) {
		if (!event) {
			SERROR("Tried to release a null event.");
			return;
		}
		ensureFMOD_OK(FMOD_Studio_EventInstance_Release(event->getInstance()), "Cannot release an fmod event.");
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

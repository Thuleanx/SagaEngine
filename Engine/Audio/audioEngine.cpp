#include "audioEngine.h"
#include "Engine/_Core/logger.h"
#include <cstdarg>
#include <iostream>

namespace Saga::AudioEngine {
	AudioImplementation implementation = AudioImplementation();

	namespace {
        void ensureFMOD_OK(FMOD_RESULT result, const char* failMessage, ...) {
			if (result != FMOD_RESULT::FMOD_OK) {
				// cryptic C code just to format the failed message with variadic arguments
				va_list arg_ptr;
				va_start(arg_ptr, failMessage);
				// TODO: Cache this buffer for less memory allocation
				char outMessage[32000] = {0}; // funny that it takes a 32000 bytes buffer to perform a small string substitution, but ig this is the C way. 
				vsnprintf(outMessage, 32000, failMessage, arg_ptr);
				va_end(arg_ptr);

				// long case to convert enum to string. I can't believe this is impossible in C++ (without altering the enum source code)
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
		bool ensureFMOD_INITIALIZED(const std::string& operation) {
			if (!implementation.studioSystem) 
				SWARN("Tried to perform operation %s when studio system is not initialize. Operation has no effect.", operation.c_str());
			return implementation.studioSystem;
		}
	}

	bool init() {
		if (implementation.studioSystem) {
			SWARN("Initialization failed. It seems like the fmod studio system has already been initialized.");
			return false;
		}
		ensureFMOD_OK(
			FMOD_Studio_System_Create(&implementation.studioSystem, FMOD_VERSION), 
			"Failed to create fmod studio system."
		);
		SINFO("Fmod Studio System Created.");
        ensureFMOD_OK(
			FMOD_Studio_System_Initialize(implementation.studioSystem, maxChannels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0), 
			"Failed to initialize fmod studio system."
		);
		SINFO("Fmod Studio System Initialized.");
		return true;
	}

	void update() {
		if (!ensureFMOD_INITIALIZED("update()")) return;
		ensureFMOD_OK(
			FMOD_Studio_System_Update(implementation.studioSystem), 
			"Failed to update fmod studio system."
		);
	}

	bool release() {
        if (!ensureFMOD_INITIALIZED("release()")) return false;
		ensureFMOD_OK(
			FMOD_Studio_System_Release(implementation.studioSystem), 
			"Failed to release fmod system."
		);
        SINFO("Fmod Studio System Released.");
		return true;
	}

	void loadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
		if (!ensureFMOD_INITIALIZED("loadBank(" + bankName +")")) return;
		SINFO("Attempting to load bank: %s.", bankName.c_str());
		if (implementation.banks.count(bankName)) {
			SWARN("Tried to load already loaded fmod bank %s. Does nothing.", bankName.c_str());
			return;
		}
		FMOD_STUDIO_BANK* bank;
		ensureFMOD_OK(
			FMOD_Studio_System_LoadBankFile(implementation.studioSystem, bankName.c_str(), flags, &bank), 
			"Can't load fmod bank \"%s\". Ensure the provided filepath is correct.", bankName.c_str()
		);
		SINFO("Loading bank success: %s.", bankName.c_str());
		if (bank) implementation.banks[bankName] = bank;
	}

	void unloadBank(const std::string& bankName) {
		if (!ensureFMOD_INITIALIZED("unloadBank(" + bankName + ")")) return;
		SINFO("Attempting to unload bank: %s.", bankName.c_str());
		if (!implementation.banks.count(bankName)) {
			SWARN("Tried to unload not loaded fmod bank %s. Please ensure correct bank name.", bankName.c_str());
			return;
		}
		// TODO: stop and release all instances related to this bank, as well as remove them from implementation, before unloading as this would invalidate all associated references.
		ensureFMOD_OK(FMOD_Studio_Bank_Unload(implementation.banks[bankName]), "Can't unload fmod bank \"%s\". Ensure the provided name is correct.", bankName.c_str());
		SINFO("Unload bank success: %s.", bankName.c_str());
		implementation.banks.erase(bankName);
	}

	void loadEvent(const std::string& eventName, bool loadSample) {
		if (!ensureFMOD_INITIALIZED("loadEvent("+ eventName+")")) return;
		if (implementation.events.count(eventName)) {
			SWARN("Tried to load already loaded fmod event %s. Please ensure the correct event name.", eventName.c_str());
			return;
		}
		FMOD_STUDIO_EVENTDESCRIPTION *description = NULL;
		ensureFMOD_OK(FMOD_Studio_System_GetEvent(implementation.studioSystem, eventName.c_str(), &description), 
			"Failed to load event: %s.", eventName.c_str());
		implementation.events[eventName] = description;
		// beware that this loads asynchronously
		if (loadSample) loadSampleData(eventName);
	}

	void loadSampleData(const std::string& eventName) {
		if (!ensureFMOD_INITIALIZED("loadSampleData("+ eventName+")")) return;
		if (!implementation.events.count(eventName)) {
			SWARN("Fmod event \"%s\" has not been loaded. Loading the event", eventName.c_str());
			loadEvent(eventName);
		}
		ensureFMOD_OK(
			FMOD_Studio_EventDescription_LoadSampleData(implementation.events[eventName]), "Can't load sample data for event %s.", eventName.c_str()
		);
	}

	void unloadSampleData(const std::string& eventName) {
		if (!ensureFMOD_INITIALIZED("unloadSampleData("+ eventName+")")) return;
		if (!implementation.events.count(eventName)) {
			SWARN("Fmod event \"%s\" has not been loaded. No effect takes place.", eventName.c_str());
			return;
		}
		ensureFMOD_OK(
			FMOD_Studio_EventDescription_UnloadSampleData(implementation.events[eventName]), "Can't unload sample data for event %s.", eventName.c_str()
		);
	}


	AudioEventInstance playEvent(const std::string& eventName) {
        if (!ensureFMOD_INITIALIZED("playEvent(" + eventName + ")")) return NULL;
		AudioEventInstance sharedInstance = createInstance(eventName);
        playEvent(sharedInstance);
		return sharedInstance;
	}

	AudioEventInstance createInstance(const std::string& eventName) {
        if (!ensureFMOD_INITIALIZED("playEvent(" + eventName + ")")) return NULL;
		if (!implementation.events.count(eventName)) {
			SWARN("Fmod event \"%s\" has not been loaded. Loading the event", eventName.c_str());
			loadEvent(eventName);
		}
		FMOD_STUDIO_EVENTINSTANCE *instance = NULL;
		// also asynchronously load the sample data, if non is loaded
		ensureFMOD_OK(FMOD_Studio_EventDescription_CreateInstance(implementation.events[eventName], &instance), 
			"Failed to create event instance for event %s.", eventName.c_str());
		return AudioEventInstance(instance);
	}

	void playEvent(AudioEventInstance instance) {
		if (!ensureFMOD_INITIALIZED("playEvent(unknown_instance)")) return;
		if (!instance) { SERROR("Tried to start a null event."); return; }
		ensureFMOD_OK(FMOD_Studio_EventInstance_Start(instance), "Failed to start fmod event.");
	}

    void stopEvent(AudioEventInstance event, bool immediate) {
		if (!ensureFMOD_INITIALIZED("stopEvent(unknown_instance)")) return;
		if (!event) { SERROR("Tried to stop a null event."); return; }
		FMOD_STUDIO_STOP_MODE mode = immediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
		ensureFMOD_OK(
			FMOD_Studio_EventInstance_Stop(event, mode), 
			"Cannot stop an fmod event."
		);
	}

	void releaseEvent(AudioEventInstance event) {
		if (!ensureFMOD_INITIALIZED("releaseEvent(unknown_instance)")) return;
		if (!event) { SERROR("Tried to release a null event."); return;}
		ensureFMOD_OK(
			FMOD_Studio_EventInstance_Release(event), 
			"Cannot release an fmod event."
		);
	}

	void releaseAllEventInstances(const std::string& eventName) {
		if (!ensureFMOD_INITIALIZED("releaseAllEventInstance(" + eventName + ")")) return;
		if (!implementation.events.count(eventName)) {
			SWARN("Tried to release instances of event %s that has not been loaded.", eventName.c_str());
			return;
		}
		ensureFMOD_OK(FMOD_Studio_EventDescription_ReleaseAllInstances(implementation.events[eventName]), "Fail to release instance for event \"%s\".", eventName.c_str());
	}

	void unloadEvent(const std::string& eventName, bool unloadSample) {
		if (!ensureFMOD_INITIALIZED("unloadEvent(" + eventName + ")")) return;
		if (!implementation.events.count(eventName)) {
			SWARN("Tried to unload event %s that has not been loaded.", eventName.c_str());
			return;
		}
		// this doesn't neccessarily unload until all instances of the event is released, so beware
		if (unloadSample) ensureFMOD_OK(FMOD_Studio_EventDescription_UnloadSampleData(implementation.events[eventName]), "Can't unload sample data for event %s.", eventName.c_str());
		implementation.events.erase(eventName);
	}

	void setParameter(AudioEventInstance instance, const std::string& parameterName, float value) {
		if (!ensureFMOD_INITIALIZED("setParameter(instance, " + parameterName + ", " + std::to_string(value) + ")")) return;
		if (!instance) { SERROR("Cannot operate on a null event instance!"); return; }
		ensureFMOD_OK(
			FMOD_Studio_EventInstance_SetParameterByName(instance, parameterName.c_str(), value, false),
			"Can't set parameter with name %s. Check to see if they are of the right name and type.", parameterName.c_str()
		);
	}

	ParameterValue getParameter(AudioEventInstance instance, const std::string& parameterName) {
        if (!ensureFMOD_INITIALIZED("getParameter(instance, " + parameterName + ")")) return ParameterValue{0,0};
		if (!instance) { SERROR("Cannot operate on a null event instance!"); return ParameterValue{0,0}; }
		float value, finalValue;
		ensureFMOD_OK(
			FMOD_Studio_EventInstance_GetParameterByName(instance, parameterName.c_str(), &value, &finalValue),
			"Can't get parameter named %s. Check to see if the parameter name and type are correct.", parameterName.c_str()
		);
		return ParameterValue{value, finalValue};
	}

	void setLabeledParameter(AudioEventInstance instance, const std::string& parameterName, std::string label) {
		if (!ensureFMOD_INITIALIZED("setLabeledParameter(instance, " + parameterName + ", " + label + ")")) return;
		if (!instance) { SERROR("Cannot operate on a null event instance!"); return; }
		ensureFMOD_OK(
			FMOD_Studio_EventInstance_SetParameterByNameWithLabel(instance, parameterName.c_str(), label.c_str(), false),
			"Can't set parameter %s of instance to label %s. Check if both are valid names.", parameterName.c_str(), label.c_str()
		);
	}

	std::string getLabeledParameter(AudioEventInstance instance, const std::string& parameterName) {
        if (!ensureFMOD_INITIALIZED("getLabeledParameter(instance, "+ parameterName + ")")) return "";
		if (!instance) { SERROR("Cannot operate on a null event instance!"); return ""; }
		float value = getParameter(instance, parameterName).value;
		FMOD_STUDIO_EVENTDESCRIPTION *description;
		ensureFMOD_OK(
			FMOD_Studio_EventInstance_GetDescription(instance, &description),
			"Cannot get the description of an audio event instance"
		);
		char* label; int sz;
		// WARN: It's a little dangerous not rounding value, but it should be ok if value is small.
		ensureFMOD_OK(
			FMOD_Studio_EventDescription_GetParameterLabelByName(description, parameterName.c_str(), 
				(int) value, label, 0, &sz),
			"Cannot get the label for parameter %s of value %d.", parameterName.c_str(), (int) value
		);
		if (!label) return "";
		return std::string(label);
	}

	void setGlobalParameter(const std::string& parameterName, float value) {
		if (!ensureFMOD_INITIALIZED("setGlobalParameter(" + parameterName + ", " + std::to_string(value) + ")")) return;
		ensureFMOD_OK(
			FMOD_Studio_System_SetParameterByName(implementation.studioSystem, parameterName.c_str(), value, false),
			"Can't set global parameter with name %s. Check to see if they are of the right name and type.", parameterName.c_str()
		);
	}

	ParameterValue getGlobalParameter(const std::string& parameterName) {
        if (!ensureFMOD_INITIALIZED("getParameter(instance, " + parameterName + ")")) return ParameterValue{0,0};
		float value, finalValue;
		ensureFMOD_OK(
			FMOD_Studio_System_GetParameterByName(implementation.studioSystem, parameterName.c_str(), &value, &finalValue),
			"Can't get global parameter named %s. Check to see if the parameter name and type are correct.", parameterName.c_str()
		);
		return ParameterValue{value, finalValue};
	}

	void setGlobalLabeledParameter(const std::string& parameterName, std::string label) {
		if (!ensureFMOD_INITIALIZED("setLabeledParameter(instance, " + parameterName + ", " + label + ")")) return;
		ensureFMOD_OK(
			FMOD_Studio_System_SetParameterByNameWithLabel(implementation.studioSystem, parameterName.c_str(), label.c_str(), false),
			"Can't set global parameter %s of instance to label %s. Check if both are valid names.", parameterName.c_str(), label.c_str()
		);
	}

	std::string getGlobalLabeledParameter(const std::string& parameterName) {
        if (!ensureFMOD_INITIALIZED("getLabeledParameter(instance, "+ parameterName + ")")) return "";
		float value = getGlobalParameter(parameterName).value;
		char* label; int sz;
		// WARN: It's a little dangerous not rounding value, but it should be ok if value is small.
		ensureFMOD_OK(
			FMOD_Studio_System_GetParameterLabelByName(implementation.studioSystem, parameterName.c_str(), (int) value, label, 0, &sz),
			"Cannot get the global label for parameter %s of value %d.", parameterName.c_str(), (int) value
		);
		if (!label) return "";
		return std::string(label);
	}

	void setListenerData(const EventAttributes &attributes) {
		if (!ensureFMOD_INITIALIZED("setListenerData()")) return;
		// creating object per frame is rather inefficient, but I assume that this would be passed in as a created object anyways
		FMOD_3D_ATTRIBUTES attr{
			vectorToFmod(attributes.position),
			vectorToFmod(attributes.velocity),
			vectorToFmod(attributes.forward),
			vectorToFmod(attributes.up)
		};
		// we assume only one listener and the attenuation position is at position in the 3d attributes
		ensureFMOD_OK(
			FMOD_Studio_System_SetListenerAttributes(implementation.studioSystem, 0, &attr, nullptr),
			"Can't set the listener attributes"
		);
	}

	void set3DAttributes(AudioEventInstance instance, const EventAttributes &attributes) {
		if (!ensureFMOD_INITIALIZED("set3DAttributes()")) return;
		// creating object per frame is rather inefficient, but I assume that this would be passed in as a created object anyways
		FMOD_3D_ATTRIBUTES attr{
			vectorToFmod(attributes.position),
			vectorToFmod(attributes.velocity),
			vectorToFmod(attributes.forward),
			vectorToFmod(attributes.up)
		};
		ensureFMOD_OK(
			FMOD_Studio_EventInstance_Set3DAttributes(instance, &attr),
			"Can't set a 3D event attribute."
		);
	}

    FMOD_VECTOR vectorToFmod(const glm::vec3& vPosition) { return FMOD_VECTOR{vPosition.x, vPosition.y, vPosition.z}; }

	EventAttributes::EventAttributes() {}
	EventAttributes::EventAttributes(glm::vec3 position, glm::vec3 velocity, glm::vec3 forward, glm::vec3 up) : position(position), velocity(velocity), forward(forward), up(up) {}
}

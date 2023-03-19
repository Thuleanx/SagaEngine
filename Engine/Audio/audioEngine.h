#pragma once

#include "fmod.h"
#include "fmod_studio.h"
#include <map>
#include <string>
#include <glm/vec3.hpp>
#include "audioEventInstance.h"
#include <memory>

namespace Saga {
	struct AudioImplementation {
		FMOD_STUDIO_SYSTEM* studioSystem;

		typedef std::map<std::string, FMOD_SOUND*> SoundMap;
		typedef std::map<int, FMOD_CHANNEL*> ChannelMap;
		typedef std::map<std::string, FMOD_STUDIO_EVENTDESCRIPTION *> EventMap;
		typedef std::map<std::string, FMOD_STUDIO_BANK*> BankMap;

		BankMap banks;
		EventMap events;
		SoundMap sounds;
		ChannelMap channels;
	};

	namespace AudioEngine {
		extern AudioImplementation implementation;

		const int maxChannels = 32;

		bool init();
		void update();
		bool release();

		// banks
		void loadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMOD_STUDIO_LOAD_BANK_NORMAL);
		void unloadBank(const std::string& bankName);

		// events
		void loadEvent(const std::string& eventName);
		std::shared_ptr<AudioEventInstance> playEvent(const std::string& eventName);
		void playEvent(std::shared_ptr<AudioEventInstance> event);
		void stopEvent(std::shared_ptr<AudioEventInstance> event, bool immediate = false);
		void releaseEvent(std::shared_ptr<AudioEventInstance> event);
		void releaseAllEventInstances(const std::string& eventName);
		void unloadEvent(const std::string& eventName);

		// sound
		void loadsound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
		void unLoadSound(const std::string& strSoundName);
		void playSound(const std::string& strSoundName, const glm::vec3& vPos = glm::vec3{ 0, 0, 0 }, float fVolumedB = 0.0f);
		void stopChannel(int nChannelId);
		void geteventParameter(const std::string& eventName, const std::string& strEventParameter, float* parameter);
		void setEventParameter(const std::string& eventName, const std::string& strParameterName, float fValue);
		void stopAllChannels();
		void setChannel3dPosition(int nChannelId, const glm::vec3& vPosition);
		void setChannelvolume(int nChannelId, float fVolumedB);
		bool isPlaying(int nChannelId);
		bool isEventPlaying(const std::string& eventName);
		float dbToVolume(float db);
		float volumeTodb(float volume);

		void set3dListenerAndOrientation(const glm::vec3& vPos = glm::vec3{ 0, 0, 0 }, float fVolumedB = 0.0f);
		FMOD_VECTOR vectorToFmod(const glm::vec3& vPosition);
	}
};

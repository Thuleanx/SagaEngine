#pragma once

#include "fmod.h"
#include "fmod_studio.h"
#include <map>
#include <string>
#include <glm/vec3.hpp>

namespace Saga {
	struct AudioImplementation {
		FMOD_STUDIO_SYSTEM* studioSystem;

		typedef std::map<std::string, FMOD_SOUND*> SoundMap;
		typedef std::map<int, FMOD_CHANNEL*> ChannelMap;
		typedef std::map<std::string, FMOD_STUDIO_EVENTINSTANCE *> EventMap;
		typedef std::map<std::string, FMOD_STUDIO_BANK_INFO*> BankMap;

		BankMap banks;
		EventMap events;
		SoundMap sounds;
		ChannelMap channels;
	};

	namespace AudioEngine {
		extern AudioImplementation implementation;

		const int maxChannels = 2;

		bool init();
		void update();
		bool release();

		void loadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
		void loadEvent(const std::string& strEventName);
		void loadsound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
		void unLoadSound(const std::string& strSoundName);
		void set3dListenerAndOrientation(const glm::vec3& vPos = glm::vec3{ 0, 0, 0 }, float fVolumedB = 0.0f);
		void playSound(const std::string& strSoundName, const glm::vec3& vPos = glm::vec3{ 0, 0, 0 }, float fVolumedB = 0.0f);
		void playEvent(const std::string& strEventName);
		void stopChannel(int nChannelId);
		void stopEvent(const std::string& strEventName, bool bImmediate = false);
		void geteventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter);
		void setEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);
		void stopAllChannels();
		void setChannel3dPosition(int nChannelId, const glm::vec3& vPosition);
		void setChannelvolume(int nChannelId, float fVolumedB);
		bool isPlaying(int nChannelId);
		bool isEventPlaying(const std::string& strEventName);
		float dbToVolume(float db);
		float volumeTodb(float volume);
		FMOD_VECTOR vectorToFmod(const glm::vec3& vPosition);
	}
};

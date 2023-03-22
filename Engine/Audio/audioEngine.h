#pragma once

#include "fmod.h"
#include "fmod_studio.h"
#include <map>
#include <string>
#include <glm/vec3.hpp>
#include "audioEventInstance.h"
#include <memory>

namespace Saga {
	/// @brief Stores data for implementing the Audio Engine.
	///		This includes various maps to fmod events and banks, as well as a reference to FMOD_STUDIO_SYSTEM.
	struct AudioImplementation {
		/// @brief FMOD studio system used to interact with Fmod studio
		FMOD_STUDIO_SYSTEM* studioSystem = nullptr;

		typedef std::map<std::string, FMOD_STUDIO_EVENTDESCRIPTION *> EventMap;
		typedef std::map<std::string, FMOD_STUDIO_BANK*> BankMap;

		/// @brief Maps bank names (filename relative to run directory) to bank pointers.
		BankMap banks;
		/// @brief Maps event names (Usually in the form: "event:/Name") as strings to event descriptions, useful for spawning new event instances.
		EventMap events;
	};

	/// @brief This contains a collection of methods that interfaces with the FMOD studio API to play audio.
	namespace AudioEngine {
		/// @brief Data used for operating the AudioEngine.
		extern AudioImplementation implementation;

		/// @brief Value of a parameter in FMOD. Whenever a parameter is retrieved from an event (or globally), it will be in this form.
		struct ParameterValue {
			/// @brief value of the parameter that's set by the API
			float value;
			/// @brief final value after automation, modulation, seek speed, and parameter velocity are taken into account. 
			///  Ideally, you want to use this value to calculate how much actual effect is applied by this parameter.
			float finalValue;
		};

		/// @brief A collection of vectors that specify an object in 3D space. This allows the AudioEngine to determine where listeners as well as AudioEventInstance are situated in space.
		struct EventAttributes {
			glm::vec3 position, velocity, forward, up; 
		};

		/**
		 * @brief Initialize the Audio Engine. Must be called before any other audio operation.
		 * 
		 * @return true if initilization is successful.
		 * @return false if initialization fails. This can be either because the initialization itself fails, or the system has already been initialized.
		 */
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

		// parameters
		// event instance
		void setParameter(AudioEventInstance instance, const std::string& parameterName, float value);
		ParameterValue getParameter(AudioEventInstance instance, const std::string& parameterName);
		void setLabeledParameter(AudioEventInstance instance, const std::string& parameterName, std::string label);
		std::string getLabeledParameter(std::shared_ptr<AudioEventInstance> instance, const std::string& parameterName);
		// global
		void setGlobalParameter(const std::string& parameterName, float value);
		ParameterValue getGlobalParameter(const std::string& parameterName);
		void setGlobalLabeledParameter(const std::string& parameterName, std::string label);
		std::string getGlobalLabeledParameter(const std::string& parameterName);

		// updating 3d orientations
		void setListenerData(const EventAttributes &attributes);
		void set3DAttributes(std::shared_ptr<AudioEventInstance> instance, EventAttributes &attributes);

		FMOD_VECTOR vectorToFmod(const glm::vec3& vPosition);
	}
};

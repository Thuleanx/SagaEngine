#pragma once

#include "fmod.h"
#include "fmod_studio.h"
#include <map>
#include <string>
#include <glm/vec3.hpp>
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

	/**
	 * @brief An audio instance equivalent to an instance of a sound. Can be start and stop. 
	 * For correct 3D positioning of an instance, make sure to update the 3D position every frame.
	 */
	using AudioEventInstance = FMOD_STUDIO_EVENTINSTANCE*;

	/** 
	 * @brief This contains a collection of methods that interfaces with the FMOD studio API to play audio.
	 */
	namespace AudioEngine {
		/// @brief Data used for operating the AudioEngine.
		extern AudioImplementation implementation;

		/**
		 * @brief Maximum channels we will use for FMOD. Usually you don't need that many (probably 4-5 would be ok).
		 */
		const int maxChannels = 32;

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

			/**
			 * @brief Construct a new Event Attributes object.
			 * 
			 */
			EventAttributes();

			/**
			 * @brief Construct a new Event Attributes object.
			 * 
			 * @param position position of the event.
			 * @param velocity velocity of the object emitting this event.
			 * @param forward forward direction of the event.
			 * @param up the up direction of the event.
			 */
			EventAttributes(glm::vec3 position, glm::vec3 velocity, glm::vec3 forward, glm::vec3 up);
		};

		/**
		 * @brief Initialize the Audio Engine. Must be called before any other audio operation.
		 * 
		 * @return true if initilization is successful.
		 * @return false if initialization fails. This can be either because the initialization itself fails, or the system has already been initialized.
		 */
		bool init();

		/**
		 * @brief Update the Audio Engine. Needs to be run every frame for accurate result.
		 * @note For accurate positioning of sounds, update both the listener's 3D position and event instance positions before calling this update.
		 */
		void update();

		/**
		 * @brief Release the Audio Engine. Must be called after init, and no audio operation should work after this call.
		 * 
		 * @return true if release was sucessful.
		 * @return false otherwise. This can fail because either the studio system is not initialized, or some failure in the fmod system.
		 */
		bool release();

		/**
		 * @brief Load a bank of audio. By default, sample data will not be loaded. 
		 * It is important that both the bank files and bank's strings file are loaded before events can be played.
		 * 
		 * @param bankFileName the filepath of the bank. This should be relative to your project source.
		 * @param flags flag for how the bank should load. 
		 */
		void loadBank(const std::string& bankFileName, FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMOD_STUDIO_LOAD_BANK_NORMAL);

		/**
		 * @brief Unload bank.
		 * 
		 * @param bankName the filepath of the bank. This should be relative to your project's source.
		 */
		void unloadBank(const std::string& bankName);

		// events
		/**
		 * @brief Load an event from a loaded bank. 
		 * 
		 * @param eventName name of the event, typically in the form "event:/Folder/Name".
		 * @param loadSampleData whether or not to also load sample data. 
		 * @note Loading sample data will preload the data for an event, allowing event instances to be created and immediately played without latency. 
		 * 	Sample data loading happens asynchronously, so there is a possibility sound won't be played immediately if you load an event and immediately attempt to play it.
		 */
		void loadEvent(const std::string& eventName, bool loadSampleData = false);

		/**
		 * @brief Play an event. If this event is not loaded, loadEvent will be called.
		 * 
		 * @param eventName name of the event, typically in the form "event:/Folder/Name".
		 * @return AudioEventInstance the correponding event instance.
		 */
		AudioEventInstance playEvent(const std::string& eventName);


		/**
		 * @brief Create an instance of a sound event. If this event has not been loaded, loadEvent will be called.
		 * 
		 * @param eventName name of the event, typically in the form "event:/Folder/Name".
		 * @return std::shared_ptr<AudioEventInstance> the instance.
		 */
		AudioEventInstance createInstance(const std::string& eventName);

		/**
		 * @brief Play an event.
		 * 
		 * @param event the event instance.
		 */
		void playEvent(AudioEventInstance event);

		/**
		 * @brief Stop an event that's playing.
		 * 
		 * @param event the event instance.
		 * @param immediate whether or not the event stops immediately, or with a falloff.
		 */
		void stopEvent(AudioEventInstance event, bool immediate = false);

		/**
		 * @brief Release an event instance. After this, the event instance is unusable.
		 * 
		 * @param event the event instance.
		 */
		void releaseEvent(AudioEventInstance event);

		/**
		 * @brief Release all event instances associated with this event.
		 * 
		 * @param eventName name of the event, typically in the form "event:/Folder/Name".
		 */
		void releaseAllEventInstances(const std::string& eventName);

		/**
		 * @brief Unload an event. This effectively removes the ability to create instances of the event until it is loaded again.
		 * 
		 * @param eventName name of the event, typically in the form "event:/Folder/Name".
		 * @param unloadSampleData whether or not to unload any sample data.
		 * @note the unload happens asynchronously and only when all event instances are released.
		 */
		void unloadEvent(const std::string& eventName, bool unloadSampleData = false);

		/**
		 * @brief Set a parameter on an event instance.
		 * 
		 * @param instance the event instance.
		 * @param parameterName the name of the parameter.
		 * @param value the value to set it to.
		 */
		void setParameter(AudioEventInstance instance, const std::string& parameterName, float value);

		/**
		 * @brief Get the value of an event.
		 * 
		 * @param instance the event instance.
		 * @param parameterName the name of the parameter.
		 * @return ParameterValue value of the event in FMOD studio.
		 */
		ParameterValue getParameter(AudioEventInstance instance, const std::string& parameterName);

		/**
		 * @brief Set a parameter on an event instance to a labeled value.
		 * 
		 * @param instance the event instance.
		 * @param parameterName the name of the parameter.
		 * @param label the label of the value to set the parameter to.
		 */
		void setLabeledParameter(AudioEventInstance instance, const std::string& parameterName, std::string label);

		/**
		 * @brief Get the label-value of a parameter.
		 * 
		 * @param instance the event instance.
		 * @param parameterName the name of the parameter.
		 * @return std::string the label value of a parameter.
		 */
		std::string getLabeledParameter(AudioEventInstance instance, const std::string& parameterName);

		/**
		 * @brief Set a global parameter.
		 * 
		 * @param parameterName the name of the parameter.
		 * @param value the value to set the parameter to.
		 */
		void setGlobalParameter(const std::string& parameterName, float value);

		/**
		 * @brief Get the value of a global parameter.
		 * 
		 * @param parameterName the name of the parameter.
		 * @return ParameterValue the value of the event in FMOD studio.
		 * 	@see ParameterValue
		 */
		ParameterValue getGlobalParameter(const std::string& parameterName);

		/**
		 * @brief Set the value of a global parameter by label.
		 * 
		 * @param parameterName the name of the parameter.
		 * @param label the label value to set the parameter to.
		 */
		void setGlobalLabeledParameter(const std::string& parameterName, std::string label);

		/**
		 * @brief Get the label value of a global parameter.
		 * 
		 * @param parameterName the name of the parameter.
		 * @return std::string the label value of the parameter.
		 */
		std::string getGlobalLabeledParameter(const std::string& parameterName);

		/**
		 * @brief Set the 3D position of the listener.
		 * 
		 * @param attributes the 3D attributes of the listener.
		 */
		void setListenerData(const EventAttributes &attributes);

		/**
		 * @brief Set the 3D attributes of an audio event instance.
		 * 
		 * @param instance the event instance.
		 * @param attributes the 3D attributes of the listener.
		 */
		void set3DAttributes(AudioEventInstance instance, const EventAttributes &attributes);

		/**
		 * @brief Convert a glm vector to an fmod vector.
		 * 
		 * @param vPosition the vector.
		 * @return FMOD_VECTOR the vector converted to fmod acceptable format.
		 */
		FMOD_VECTOR vectorToFmod(const glm::vec3& vPosition);
	}
};

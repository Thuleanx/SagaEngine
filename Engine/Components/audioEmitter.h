#pragma once

#include <string>
#include "../Audio/audioEngine.h"

namespace Saga {

/**
 * @brief Component for emitting sound events.
 * @ingroup component
 * @ingroup audio
 */
struct AudioEmitter {
	std::string eventName; //!< name of the event. Typically "event:/eventName".
	bool playOnAwake; //!< whether this sound should play as soon as the gameworld is awoken.
	bool preload; //!< whether we should preload the sample data for the sound so that playing it is instantaneous. Do not change this value throughout execution of the program.

	// these are values you won't need to set
	AudioEventInstance audioInstance; //!< an event instance representing the sound. When the emitter is not playing any sound, this should be nullptr.
	bool is3D; //!< determines if the sound event is 3D.

	/**
	 * @brief Construct a new Audio Emitter object.
	 */
	AudioEmitter();

	/**
	 * @brief Construct a new Audio Emitter object.
	 * 
	 * @param eventName name of the event.
	 * @param playOnAwake whether the event should play on awake.
	 * @param preload whether to preload sample data for the event.
	 */
	AudioEmitter(std::string eventName, bool playOnAwake = false, bool preload = false);
};



}; // namespace Saga

#pragma once
#include <memory>

namespace Saga {
    class GameWorld;
}

namespace Saga::Systems {
	/**
	 * @brief System that searches for cameras in the screen, and draw all shapes relative to those cameras on screen.
	 * 
	 * @ingroup system
	 * @param world 
	 */
	void drawSystem(std::shared_ptr<GameWorld> world);

	/**
	 * @brief System to be run on screen resize. This resize all cameras.
	 * 
	 * @ingroup system
	 * @param world 
	 * @param width the width of the screen, in pixels.
	 * @param height the height of the screen, in pixels.
	 */
	void drawSystem_OnResize(std::shared_ptr<GameWorld> world, int width, int height);

	/**
	 * @brief Register Groups that the drawSystem use.
	 * 
	 * @param world 
	 */
	void registerDrawSystem(std::shared_ptr<GameWorld> world);
}

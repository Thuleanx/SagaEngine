#pragma once
#include <memory>

namespace Saga {
    class GameWorld;
}

namespace Saga::Systems {

    /**
     * @brief Register the built in AI systems.
     * This includes automatic updates for behaviour trees.
     *
     * @ingroup system
     * @param world
     */
    void registerAISystems(std::shared_ptr<GameWorld> world);

    /**
     * @brief Updates AI systems in the world. This includes updating 
     * all behaviour trees, which includes running update on its root.
     *
	 * @ingroup system
	 * @param world 
	 * @param deltaTime time since last update.
	 * @param time time since start of the program.
     */
    void AIupdateSystem(std::shared_ptr<GameWorld> world, float deltaTime, float time);
}

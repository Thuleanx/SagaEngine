#pragma once

#include <vector>
#include <utility>
#include "../Gameworld/gameworld.h"

namespace Saga {

/**
 * @brief 
 * Manages several GameWorld. Responsible for assembling them and passing along Staged events as well as user input events.
 * 
 */
class App {
public:
	App();
	virtual ~App();
	/**
	 * @brief This invokes the Update stages of any world, or the Start stage if this is the world's first frame.
	 * 
	 * @param deltaTime time since the last update.
	 * @param time time ellapsed since the start of the Application.
	 */
	void update(float deltaTime, float time);

	/**
	 * @brief This invokes the FixedUpdate stages of any world, if that world has been started. 
	 * Fixed updates should run at a stable frame rate, and is a good place to put physics-based Systems.
	 * 
	 * @param deltaTime time since the last fixed update.
	 * @param time time ellapsed since the start of the Application.
	 */
	void fixedUpdate(float deltaTime, float time);
	
	/**
	 * @brief Invoke the Draw stage of any managed World.
	 */
	void draw();

	/**
	 * @brief Invoke a keyboard event.
	 * 
	 * @param key a GLFW key enum.
	 * @param action a GLFW action on that key.
	 */
	void keyEvent(int key, int action);

	/**
	 * @brief Invoke a mouse position event. This happens whenever the mouse changes position.
	 * 
	 * @param xpos the x position of the mouse in screen space.
	 * @param ypos the y position of the mouse in screen space.
	 */
    void mousePosEvent(double xpos, double ypos);

	/**
	 * @brief Invoke a mouse button event.
	 * 
	 * @param key a GLFW key enum.
	 * @param action a GLFW action on that mouse button.
	 */
    void mouseButtonEvent(int button, int action);

	/**
	 * @brief Invoke a scroll event, that happens when the user scroll with the mouse.
	 * 
	 * @param distance the distance in pixel of the scroll.
	 */
    void scrollEvent(double distance);
	
	/**
	 * @brief Invoked whenever the framebuffer needs to be resized.
	 * 
	 * @param width the width in pixels of the new framebuffer.
	 * @param height the height in pixels of the new framebuffer.
	 */
    void framebufferResizeEvent(int width, int height);

	/**
	 * @brief Invoked whenever the user resize the window.
	 * 
	 * @param width the width of the new window in pixels.
	 * @param height the height of the new window in pixels.
	 */
    void windowResizeEvent(int width, int height);

	/**
	 * @brief Create a Game World object
	 * 
	 * @return std::shared_ptr<GameWorld> 
	 */
	std::shared_ptr<GameWorld> createGameWorld();

	/**
	 * @brief Remove a GameWorld.
	 * 
	 * @param world 
	 */
	void removeGameWorld(std::shared_ptr<GameWorld> world);

private:
	/**
	 * @brief Used so that only App has exclusive right to invoke Staged Systems and Input Systems.
	 */
	class AppExclusiveGameWorld : public GameWorld {
	public:
		virtual ~AppExclusiveGameWorld();

		bool started = false;
		void runStageStartup();
		void runStageUpdate(float deltaTime, float time);
		void runStageFixedUpdate(float deltaTime, float time);
		void runStageDraw();
		void runStageCleanup();
		void keyEvent(int key, int action);
		void mousePosEvent(double xpos, double ypos);
		void mouseButtonEvent(int button, int action);
		void scrollEvent(double distance);
		void windowResizeEvent(int width, int height);
	};
	std::vector<std::shared_ptr<AppExclusiveGameWorld>> worlds;
};
}

#pragma once

#include "Graphics/global.h"
#include <GLFW/glfw3.h>


namespace Saga {

class App;

/**
 * @brief A wrapper around App that separates it from Window, the script running the game loop. 
 * This receives user events and pass them along to App.
 * 
 */
class Core {
public:
    Core();
    ~Core();
    void update(double deltaTime, double time);
    void fixedupdate(double fixedDeltaTime, double time);
    void draw();
    void keyEvent(int key, int action);
    void mousePosEvent(double xpos, double ypos);
    void mouseButtonEvent(int button, int action);
    void scrollEvent(double distance);
    void windowResizeEvent(int width, int height);
    void framebufferResizeEvent(int width, int height);

private:
	std::shared_ptr<App> application;
	double time;
};
} // namespace Saga
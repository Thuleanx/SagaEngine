#pragma once

#include "core.h"

class Window
{
public:
    Window();
    ~Window();
	void run();

private:
    void start();
    void loop();
    void end();
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* m_GLFWwindow;
    Saga::Core* m_core;
    const double m_secPerFixedUpdate = 1.0/120;
};

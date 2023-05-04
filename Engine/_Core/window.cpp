#include "window.h"
#include "logger.h"
#include <exception>
#include <iostream>
#include <memory>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

using namespace Saga;

Window::Window(){
	initializeLogger();
	SINFO("Window starting.");
}

Window::~Window(){
	delete m_core;
	shutDownLogger();
}

void Window::run() {
    start();
	SINFO("Window loop starting.");
    loop();
	SINFO("Window loop ended.");
    end();
	SINFO("Window ended.");
}

void Window::start(){
    // Testing glfw
    if(!glfwInit()){
		SFATAL("GLFW init failed :(");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    m_GLFWwindow = glfwCreateWindow(640, 480, "CS1950U Engine", NULL, NULL);
    if (!m_GLFWwindow)
    {
		SFATAL("Window creation failed :(");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_GLFWwindow);

    GraphicsEngine::Global::graphics.initializeGLEW(); // IMPORTANT: Can't make ANY OpenGL calls before this occurs!
    GraphicsEngine::Global::graphics.initialize();
    int width, height;
    glfwGetWindowSize(m_GLFWwindow, &width, &height);
    GraphicsEngine::Global::graphics.setWindowSize(glm::ivec2(width, height));
    glfwGetFramebufferSize(m_GLFWwindow, &width, &height);
    GraphicsEngine::Global::graphics.setFramebufferSize(glm::ivec2(width, height));

    glfwSwapInterval(1);

    // Set up core now that windowing and opengl are set up
	m_core = new Core();
	

    // Stores variable in glfw to reference our m_core object. This allows it to be accessed
    // even in static methods such as keyCallback and windowSizeCallback
    glfwSetWindowUserPointer(m_GLFWwindow, m_core);

    glfwSetKeyCallback(m_GLFWwindow, keyCallback);

    glfwSetMouseButtonCallback(m_GLFWwindow, mouseButtonCallback);

    // glfwSetInputMode(m_GLFWwindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // if (glfwRawMouseMotionSupported()){
    //     glfwSetInputMode(m_GLFWwindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    // }

    glfwSetCursorPosCallback(m_GLFWwindow, cursorPosCallback);

    glfwSetScrollCallback(m_GLFWwindow, scrollCallback);

    glfwSetWindowSizeCallback(m_GLFWwindow, windowSizeCallback);

    glfwSetFramebufferSizeCallback(m_GLFWwindow, framebufferSizeCallback);

    glfwSetInputMode(m_GLFWwindow, GLFW_STICKY_KEYS, GLFW_TRUE);

    // Setup Imgui contet
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(m_GLFWwindow, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}


void Window::loop(){
	double startTime = glfwGetTime();
    double previous = glfwGetTime();
	double previousFixed = glfwGetTime();
    while (!glfwWindowShouldClose(m_GLFWwindow))
    {
        try {
            double currentTime = glfwGetTime();
            while (previousFixed + m_secPerFixedUpdate <= currentTime) {
                m_core->fixedupdate(m_secPerFixedUpdate, previousFixed + m_secPerFixedUpdate - startTime);
                previousFixed += m_secPerFixedUpdate;
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            double deltaTime = currentTime - previous;
            double elapsedTime = currentTime - startTime;

            glfwPollEvents();
            m_core->update(deltaTime, elapsedTime);
            m_core->draw();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(m_GLFWwindow);
            previous = currentTime;
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            fflush(stderr);
            break;
        } catch (...) {

            break;
        }
    }
}


void Window::end(){
    glfwDestroyWindow(m_GLFWwindow);
    glfwTerminate();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(key == GLFW_KEY_ESCAPE){
        glfwSetWindowShouldClose(window, true);
    }
    Core* ptr = (Core*)glfwGetWindowUserPointer(window);
    ptr->keyEvent(key, action);
}

void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos){
    Core* ptr = (Core*)glfwGetWindowUserPointer(window);
    ptr->mousePosEvent(xpos, ypos);
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    Core* ptr = (Core*)glfwGetWindowUserPointer(window);
    ptr->mouseButtonEvent(button, action);
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    Core* ptr = (Core*)glfwGetWindowUserPointer(window);
    ptr->scrollEvent(yoffset);
}

void Window::windowSizeCallback(GLFWwindow* window, int width, int height){
    Core* ptr = (Core*)glfwGetWindowUserPointer(window);
    ptr->windowResizeEvent(width, height);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height){
    Core* ptr = (Core*)glfwGetWindowUserPointer(window);
    ptr->framebufferResizeEvent(width, height);
}

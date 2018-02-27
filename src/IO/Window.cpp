#include "Window.hpp"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"

#include <iostream> /* cout, cerr */

int Window::width = DEFAULT_WIDTH;
int Window::height = DEFAULT_HEIGHT;
GLFWwindow *Window::window;

float Window::timeStep = 0.f;
int Window::FPS = 0;
double Window::lastFpsTime = 0.0;
double Window::lastFrameTime = 0.0;
double Window::runTime = 0.0;
int Window::nFrames = 0;

float Window::imGuiTimer = 1.f;
bool Window::imGuiEnabled = true;

void Window::errorCallback(int error, const char *desc) {
    std::cerr << "Error " << error << ": " << desc << std::endl;
}

void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action >= GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    Keyboard::setKeyStatus(key, action);

    if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mode);
    }
    else {
        Keyboard::setKeyStatus(key, action);
    }
}

void Window::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
    }
    else {
        Mouse::setButtonStatus(button, action);
    }
}

void Window::characterCallback(GLFWwindow *window, unsigned int c) {
    if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_CharCallback(window, c);
    }
}

int Window::init(std::string name) {
    /* Set error callback */
    glfwSetErrorCallback(errorCallback);

    /* Init GLFW */
    if (!glfwInit()) {
        std::cerr << "Error initializing GLFW" << std::endl;
        return 1;
    }

    /* Request version 3.3 of OpenGL */
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

    /* Create GLFW window */
    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    /* Init ImGui */
    ImGui_ImplGlfwGL3_Init(window, false);

    /* Set callbacks */
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    /* Init GLAD */
	if (!gladLoadGL())
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    /* Vsync */
    glfwSwapInterval(1);

    /* More init */
    update();

    return 0;
}

void Window::setTitle(const char *name) {
    glfwSetWindowTitle(window, name);
}

void Window::update() {
    /* Set viewport to window size */
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    /* Don't update display if window is minimized */
    if (!width && !height) {
        return;
    }

    /* Update mouse */
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    Mouse::update(x, y);

    /* Update timing */
    runTime = (float)glfwGetTime();
    timeStep = (float)runTime - lastFrameTime;
    lastFrameTime = runTime;
    nFrames++;
    if (runTime - lastFpsTime >= 1.0) {
        FPS = nFrames;
        nFrames = 0;
        lastFpsTime = runTime;
    }
 
    /* Update ImGui */
    imGuiTimer += timeStep;
    if (Keyboard::isKeyPressed(GLFW_KEY_GRAVE_ACCENT) && (Keyboard::isKeyPressed(GLFW_KEY_LEFT_SHIFT) ||
        Keyboard::isKeyPressed(GLFW_KEY_RIGHT_SHIFT)) && imGuiTimer > 1.f) {
        toggleImgui();
        imGuiTimer = 0.f;
    }

    if (isImGuiEnabled()) {
        ImGui_ImplGlfwGL3_NewFrame(true);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

double Window::getTime() {
    return glfwGetTime();
}

void Window::shutDown() {
    /* Clean up GLFW */
    glfwDestroyWindow(window);
    glfwTerminate();
}
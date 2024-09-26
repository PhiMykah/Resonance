#ifndef BACKEND_H
#define BACKEND_H

// GUI Headers
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <ImGuizmo/ImGuizmo.h>

// File Dialog 
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "ImGuiFileDialog/ImGuiFileDialogConfig.h"

// Primary Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <iostream>

#define WHITE glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define BLACK glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)

struct WindowData
{
    int width;
    int height;
    int windowed_width;
    int windowed_height;
    int xpos;
    int ypos;
    const char * title;
    GLFWmonitor * monitor;
    bool fullscreen;
};

GLFWwindow * initWindow(int width, int height, const char * title, GLFWmonitor *fullscreen, GLFWwindow *share = NULL);

ImGuiIO initIMGUI(GLFWwindow * window);

void closeIMGUI();

void ToggleFullscreen(GLFWwindow * window);

void OpenFileDialog();

// Callbacks
void window_size_callback(GLFWwindow* window, int width, int height);
void window_iconify_callback(GLFWwindow* window, int iconified);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
#endif // !BACKEND_H
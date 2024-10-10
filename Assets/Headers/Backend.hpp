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

// C++ headers
#include <iostream>
#include <map>

#include "Constants.hpp"

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

void initIMGUI(GLFWwindow * window);

void closeIMGUI();

void ToggleFullscreen(GLFWwindow * window);

void OpenFileDialog();

void AddFileDialog();

void glRenderSettings();

struct ScopedID {
    ScopedID(unsigned int id) {
        ImGui::PushID(id);  // Push the ID when the object is created
    }

    ~ScopedID() {
        ImGui::PopID();  // Pop the ID when the object is destroyed
    }
};

// Callbacks
void window_size_callback(GLFWwindow* window, int width, int height);
void window_iconify_callback(GLFWwindow* window, int iconified);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
#endif // !BACKEND_H
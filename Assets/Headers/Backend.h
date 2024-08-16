#ifndef BACKEND_H
#define BACKEND_H

// GUI Headers
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

#define WHITE glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define BLACK glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)

GLFWwindow * initWindow(int width, int height, const char * title, GLFWmonitor *fullscreen, GLFWwindow *share = NULL);

ImGuiIO initIMGUI(GLFWwindow * window);

void closeIMGUI();

#endif // !BACKEND_H
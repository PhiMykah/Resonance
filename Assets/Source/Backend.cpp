#include "Backend.h"

/*
Initializes GLFW window with given information
Does NOT Check for valid window or successful initialization

Parameters
----------
width : int
    The desired width, in screen coordinates, of the window. This must be greater than zero.
height : int
    The desired height, in screen coordinates, of the window. This must be greater than zero.
title : const char *
    The initial, UTF-8 encoded window title.
fullscreen : GLFWmonitor *
    The monitor to use for full screen mode, or NULL for windowed mode.
share : GLFWwindow * 
    The window whose context to share resources with, or NULL to not share resources.

Returns
-------
GLFWwindow * 
    Handle of created window, or NULL if an error arises.
*/
GLFWwindow * initWindow(int width, int height, 
                      const char * title, 
                      GLFWmonitor *fullscreen, GLFWwindow *share){

    // Give glfw information about opengl version (4.6)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // Give glfw information about opengl profile
    // Currently running the CORE profile with modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    
    return glfwCreateWindow(width, height, title, fullscreen, share);
}

/*
Initializes IMGUI, attaches IMGUI to window, 
and creates input/output handler

Parameters
----------
window : GLFWwindow*
    Target window for IMGUI

Returns
io : ImGuiIO
    Input/Output handler for IMGUI
*/
ImGuiIO initIMGUI(GLFWwindow * window){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Create input/output handler
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // Choose imgui style
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460"); // Specify initialization based on version of OpenGL
    return io;
}

/*
Shutdown IMGUI and IMGUI OpenGL Attachment

Parameters
----------
None

Returns
-------
None
*/
void closeIMGUI(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
 
// *************
// * Callbacks *
// *************

void window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void window_iconify_callback(GLFWwindow* window, int iconified)
{ 
    // The window was minimized
    if (iconified)
    {
        glfwWaitEvents();
    }
    // The window was restored
    else
    {
        glfwPollEvents();
    }
}
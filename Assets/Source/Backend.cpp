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

// *****************
// * Code Snippets *
// *****************

void ToggleFullscreen(GLFWwindow * window)
{
    WindowData * win = (WindowData *)glfwGetWindowUserPointer(window);

    if (!win->fullscreen)
    {
        // Collect current xpos and ypos for later
        glfwGetWindowPos(window, &win->xpos, &win->ypos);
        glfwGetWindowSize(window, &win->windowed_width, &win->windowed_height);

        // Get the monitor that the window is currently on (not in fullscreen)
        int count;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        const GLFWvidmode* mode;

        int windowX, windowY, monitorX, monitorY;
        glfwGetWindowPos(window, &windowX, &windowY);

        // Find the monitor that the window is on
        for (int i = 0; i < count; i++) {
            glfwGetMonitorPos(monitors[i], &monitorX, &monitorY);
            mode = glfwGetVideoMode(monitors[i]);

            if (windowX >= monitorX && windowX < monitorX + mode->width &&
                windowY >= monitorY && windowY < monitorY + mode->height) {
                win->monitor = monitors[i];
                break;
            }
        }

        // Get video mode from monitor
        mode = glfwGetVideoMode(win->monitor);

        // Set window to full screen based on monitor
        glfwSetWindowMonitor(window, win->monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

    }
    else {
        glfwSetWindowMonitor(window, NULL, win->xpos, win->ypos, win->windowed_width, win->windowed_height, 0);
    }
    win->fullscreen = !win->fullscreen;
}

void OpenFileDialog()
{
    IGFD::FileDialogConfig config;
    config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".*,.fid,.ft2,.ft3,.ft4", config);
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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // *************
    // * SHORTCUTS *
    // *************

    // Ensure right click isnt held during shortcuts
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS) {
        // FULLSCREEN HOTKEY
        // Check if both Alt and Enter are pressed
        if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && (mods & GLFW_MOD_ALT)) {
            ToggleFullscreen(window);
        }
        // OPEN FILE HOTKEY
        // Check if both O and CTRL are pressed
        if (key == GLFW_KEY_O && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
            OpenFileDialog();
        }
    }
}

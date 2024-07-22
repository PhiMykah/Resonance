#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Color
{
    float R;
    float G;
    float B;
    float A;
    Color(float r, float g, float b, float a){
        R = r;
        G = g;
        B = b;
        A = a;
    }
};

int main() {
    // *******************
    // * Initialize glfw *
    // *******************
    glfwInit();

    // Give glfw information about opengl version (3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Give glfw information about opengl profile
    // Currently running the CORE profile with modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Dimensions of window
    int width = 800;
    int height = 800;

    const char * win_name = "Test Window"; // Window name
    // Designated fullscreen monitor 
    GLFWmonitor * fullscreen = NULL; //glfwGetPrimaryMonitor(); 

    // Initialize window with the following parameters:
    //  width, height, name, fullscreen monitor, and context object sharing
    GLFWwindow * main_window = glfwCreateWindow(width, height, win_name, fullscreen, NULL); 

    // Since window returns null if the window fails to initialize, check for failure
    if (main_window == NULL) 
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate(); // Terminate glfw processes
        return -1;
    }
    glfwMakeContextCurrent(main_window); // Create window

    // *****************
    // * Initialize GL *
    // *****************
    gladLoadGL();
    
    // Set openGL to render from top left corner (0,0) to bottom right corner (width, height)
    glViewport(0,0, width, height);
    
    // Set Background color
    // Rebecca Purple: (0.4f, 0.2f, 0.6f, 1.0f) 
    // Navy Blue: (0.07f, 0.13f, 0.17f, 1.0f)
    Color bg_color(0.4f, 0.2f, 0.6f, 1.0f); 
    // Clear back buffer and set it to color with RGBA float values
    glClearColor(bg_color.R, bg_color.G, bg_color.B, bg_color.A);
    // Execute command above on color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    // Swap back buffer and front buffer for effects to take place
    glfwSwapBuffers(main_window);

    // While loop repeats until window is told to close or user closes window
    while(!glfwWindowShouldClose(main_window)) {
        // Update window events every loop, such as resizing, moving, min-max, and other events
        glfwPollEvents();
    }

    glfwDestroyWindow(main_window); // Close window when complete
    glfwTerminate(); // Terminate glfw process
    return 0;
}

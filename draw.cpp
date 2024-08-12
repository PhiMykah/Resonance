// Backend Headers and Source Code
#include "Backend.h"

// Object Headers
#include "Mesh.h"

// C/C++ Library Headers
#include <iostream>
#include <math.h>

// Other Headers
#include "Shapes.h"

// NMRPype Headers
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "rd/fdatap.h"
#include "rd/memory.h"
#include "rd/cmndargs.h"
#include "rd/specunit.h"
#include "rd/readnmr.h"
#include "rd/inquire.h"
#include "rd/vutil.h"
#include "rd/prec.h"
#include "rd/rand.h"
#include "rd/atof.h"

struct NMRVertex
{
    glm::vec3 coordinates;
    float intensity;
};


int main( int argc, char **argv )
{
    // GLFW parameters

    int width = 800; // Window width
    int height = 800; // Window height
    const char *title = "Resonance"; // Window name
    GLFWmonitor *fullscreen = NULL; // Use `glfwGetPrimaryMonitor()` for fullscreen

    glfwInit();
    // Initialize glfw window with the following parameters:
    //  width, height, name, fullscreen monitor, and context object sharing
    GLFWwindow *main_window = initWindow(width, height, title, fullscreen);

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
    glViewport(0, 0, width, height);

    // Initialize IMGUI 
    ImGuiIO io = initIMGUI(main_window);

    // *****************************
    // * Initialize Loop Variables *
    // *****************************

    // Set Background color
    // Rebecca Purple: (0.4f, 0.2f, 0.6f, 1.0f)
    // Navy Blue: (0.07f, 0.13f, 0.17f, 1.0f)
    glm::vec4 bg_color = glm::vec4((float)(28.0 / 255.0), (float)(30.0 / 255.0), (float)(41.0 / 255.0), 1.0f);

    // Shape attributes
    // -------------------
    bool drawShape = true; // Shape visibility

    // Ignore mouse inputs with imGUI enabled
    // if (!io.WantCaptureMouse) {
    //     // Handle input
    // }

    // Initialize camera view
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    // Enable depth testing in OpenGL rendering
    glEnable(GL_DEPTH_TEST);

    // While loop repeats until window is told to close or user closes window
    while (!glfwWindowShouldClose(main_window))
    {
        // Clear back buffer and set it to color with RGBA float values
        glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);

        // Clear Color and Depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create new UI Frame, inform opengl3 and glfw
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // shader_program.Activate(); // Activate program

        // Set input for camera
        camera.Input(main_window);

        // Update camera matrix based on view plane and FOV
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

        // Create UI Window
        ImGui::Begin("Shape Settings");                // ImGUI window creation
        ImGui::Text("Do you like this shape?");        // Text that appears in the window
        ImGui::Checkbox("Draw Shape", &drawShape);     // Select whether to draw the shape
        ImGui::End();

        // Render UI Window
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the buffers to update the screen each frame !!
        glfwSwapBuffers(main_window);
        // Update window events every loop, such as resizing, moving, min-max, and other events
        glfwPollEvents();
    }

    // *****************************
    // * Deletion and Deallocation *
    // *****************************
    
    closeIMGUI(); // Close ImGui and remove GL link
    glfwDestroyWindow(main_window); // Close window when complete
    glfwTerminate();           // Terminate glfw process

    return 0;
}

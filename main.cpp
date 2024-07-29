// GUI Headers
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Primary Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Shader Headers
#include "Assets/Shaders/shaderClass.h"
#include "Assets/Buffers/VAO.h"
#include "Assets/Buffers/VBO.h"
#include "Assets/Buffers/EBO.h"

// C/C++ Library Headers
#include <iostream>
#include <math.h> 

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

// *******************
// * Define Triangle *
// *******************

// Initialize triangle vertex array
GLfloat vertices[] =
{ //        X,Y,Z Coordinates                       /       RGB Color        //
    -0.5f,  -0.5f * float(sqrt(3)) / 3,     0.0f,       0.80f, 0.30f, 0.02f, // Bottom left corner
     0.5f,  -0.5f * float(sqrt(3)) / 3,     0.0f,       0.80f, 0.30f, 0.02f, // Bottom right corner
     0.0f,   0.5f * float(sqrt(3)) * 2 / 3, 0.0f,       1.00f, 0.60f, 0.32f, // Top vertex
    -0.25f,  0.5f * float(sqrt(3)) / 6,     0.0f,       0.90f, 0.45f, 0.17f, // Mid left point
     0.25f,  0.5f * float(sqrt(3)) / 6,     0.0f,       0.90f, 0.45f, 0.17f, // Mid right Point
     0.0f,  -0.5f * float(sqrt(3)) / 3,     0.0f,       0.80f, 0.30f, 0.02f  // Bottom Center point
};

// Initialize Index buffer to tell vertex shader the order to form primitives
GLuint indices[] = 
{
    0, 3, 5, // Lower left triangle uses vertices 0, 3, and 5   
    3, 2, 4, // Lower right triangle uses vertices 3, 2, and 4
    5, 4, 1, // Top triangle uses vertices 5, 4, and 1
};

int main() {
    // *******************
    // * Initialize glfw *
    // *******************
    glfwInit();

    // Give glfw information about opengl version (4.6)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // Give glfw information about opengl profile
    // Currently running the CORE profile with modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Dimensions of window
    int width = 800;
    int height = 800;

    const char * win_name = "Triforce Simulator"; // Window name
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

    // *************************************************
    // * Initialize Shaders, Vertex Array, and Buffers *
    // *************************************************
    // Vertex array object MUST be created before vertex buffer object
    // Vertex buffer is a different kind of buffer than the front and back buffer

    Shader shaderProgram("Assets/Shaders/default.vert", "Assets/Shaders/default.frag");
    // Vertex Array Object (VAO)
    VAO vao1;
    vao1.Bind();

    // Vertex Buffer Object (VBO)
    VBO vbo1(vertices, sizeof(vertices));
    // Index Buffer Object (EBO)
    EBO ebo1(indices, sizeof(indices));

    // Link vbo layouts to corresponding vao
    // Position Coordinate layout (layout 0)
    vao1.LinkAttrib(vbo1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
    // Color layout (layout 1)
    vao1.LinkAttrib(vbo1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // Unbind vao, vbo, and ebo to avoid further modifications
    vao1.Unbind();
    vbo1.Unbind();
    ebo1.Unbind();

    // ********************
    // * Initialize IMGUI *
    // ********************
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Create input/output handler
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Choose imgui style
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(main_window, true);
    ImGui_ImplOpenGL3_Init("#version 460"); // Specify initialization based on version of OpenGL

    // *****************************
    // * Initialize Loop Variables *
    // *****************************

    // Set Background color
    // Rebecca Purple: (0.4f, 0.2f, 0.6f, 1.0f) 
    // Navy Blue: (0.07f, 0.13f, 0.17f, 1.0f)
    Color bg_color(0.4f, 0.2f, 0.6f, 1.0f); 

    // Triangle attributes
    // -------------------
    bool drawTriangle = true; // Triangle visibility
    float size = 1.0f; // Triangle size
    // float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Triangle color

    // Ignore mouse inputs with imGUI enabled
    // if (!io.WantCaptureMouse) {
    //     // Handle input
    // }

    // Pointer to uniform variable size from shader
    GLuint sizeID = glGetUniformLocation(shaderProgram.ID, "size");
    // GLunit colorID = glGetUniformLocation(shaderProgram.ID, "color");

    // Export variables to shaders
    shaderProgram.Activate();
    glUniform1f(sizeID, size); // glUniform changes based on the the inputted datatype
    // glUniform4f(colorID, color[0], color[1], color[2], color[3]);

    // While loop repeats until window is told to close or user closes window
    while(!glfwWindowShouldClose(main_window)) {
        // Clear back buffer and set it to color with RGBA float values
        glClearColor(bg_color.R, bg_color.G, bg_color.B, bg_color.A);
        // Execute command above on color buffer
        glClear(GL_COLOR_BUFFER_BIT);

        // Create new UI Frame, inform opengl3 and glfw
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Update uniforms with ImGui values
        shaderProgram.Activate(); // Activate program
        glUniform1f(sizeID, size);
        // glUniform4f(colorID, color[0], color[1], color[2], color[3]);
        
        vao1.Bind(); // Bind VAO

        // Draw the Triforce
        //  - Select Triangle primitive
        //  - Specify number of indices 
        //  - Specify datatype of indices
        //  - Identify index of indices
        if (drawTriangle){ 
            glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        }

        // Create UI Window 
        ImGui::Begin("Triangle Settings"); // ImGUI window creation
        ImGui::Text("Do you like the triangle?"); // Text that appears in the window
        ImGui::Checkbox("Draw Triangle", &drawTriangle); // Select whether to draw the triangle
        ImGui::SliderFloat("Size", &size, 0.5f, 2.0f); // Size slider that appears in the window
        // ImGui::ColorEdit4("Color", color); // Fancy color editor that appears in the window
        ImGui::End();

        // Render UI Window
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the buffers to update the screen each frame !! 
        glfwSwapBuffers(main_window);
        // Update window events every loop, such as resizing, moving, min-max, and other events
        glfwPollEvents();
    }
    
    // Close ImGui and Delete Objects
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Delete GL objects created
    vao1.Delete();
    vbo1.Delete();
    ebo1.Delete();
    shaderProgram.Delete();

    glfwDestroyWindow(main_window); // Close window when complete
    glfwTerminate(); // Terminate glfw process
    return 0;
}

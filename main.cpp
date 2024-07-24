// GUI Headers
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Primary Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

// Vertex Shader source code
const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform float size;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(size * aPos.x, size * aPos.y, size * aPos.z, 1.0);\n"
"}\0";
// Fragment Shader source code
const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"   FragColor = color;\n"
"}\n\0";

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
    
    // Initialize vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Provide source to vertex shader
    // Parameters:
    // reference value, screen count, source code, array of string lengths
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // Compile source code to machine code
    glCompileShader(vertexShader);

    // Initialize fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Provide source to fragment shader
    // Parameters:
    // reference value, screen count, source code, array of string lengths
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    // Compile source code to machine code
    glCompileShader(fragmentShader);

    // Create shader program , attach, then wrap shaders into shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Remove shaders now that the shaders are loaded to the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // **************************************
    // * Initialize Vertex Array and Buffer *
    // **************************************
    // Vertex array object MUST be created before vertex buffer object
    // Vertex buffer is a different kind of buffer than the front and back buffer

    // Initialize triangle vertex array
    GLfloat vertices[] =
    {
        -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Bottom left corner
        0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Bottom right corner
        0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f, // Top vertex
        -0.5f / 2 , 0.5f * float(sqrt(3)) / 6, 0.0f, // Mid left point
        0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // Mid right Point
        0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f // Bottom Center point
    };

    // Initialize Index buffer to tell vertex shader the order to form primitives
    GLuint indices[] = 
    {
        0, 3, 5, // Lower left triangle uses vertices 0, 3, and 5
        3, 2, 4, // Lower right triangle uses vertices 3, 2, and 4
        5, 4, 1, // Top triangle uses vertices 5, 4, and 1
    };

    // Vertex Array Object (VAO)
    GLuint VAO;

    // Vertex Buffer Object (VBO)
    GLuint VBO;
    
    // Index Buffer Object (EBO)
    GLuint EBO;

    // Generate VAO, then VBO, then EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Set VBO to current object, or the binded object
    // GL_ARRAY_BUFFER is the vertex buffer type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Place vertices into the buffer.
    //  - Specify buffer type
    //  - Allocate buffer size based on the size of the data passed
    //  - Pass data to the buffer
    //  - Data usage (STREAM, STATIC, DYNAMIC)
    //      - STREAM suggests the data will be initialized once and used a few times
    //      - STATIC suggests the data will be initialized once and used many times
    //      - DYNAMIC suggests the data will be initialized and used many times
    //  - Specify data usage action (DRAW, READ, COPY) 
    //      - DRAW suggests the data will be drawn on the screen
    //      - READ suggests the data will be read 
    //      - COPY suggests that the data will be copied
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind EBO
    //   - Buffer type for index buffer is GL_ELEMENT_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // Place index array into buffer
    // See vertex buffer data above for more information
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Configure VAO with VBO
    // - Position of Vertex Attribute
    // - Number of Values per Vertex
    // - Value type
    // - Identify whether or not values are integers
    // - Size of data per vertex
    // - Offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Enable Vertex Attribute Array
    glEnableVertexAttribArray(0);
    
    // Reset array buffer, vertex buffer, and index buffer to zero IN THAT ORDER to avoid modifying 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
    float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Triangle color

    // Ignore mouse inputs with imGUI enabled
    // if (!io.WantCaptureMouse) {
    //     // Handle input
    // }

    // Export variables to shaders
    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, "size"), size);
    glUniform4f(glGetUniformLocation(shaderProgram, "color"), color[0], color[1], color[2], color[3]);

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
        glUseProgram(shaderProgram); // Activate program
        glUniform1f(glGetUniformLocation(shaderProgram, "size"), size);
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), color[0], color[1], color[2], color[3]);
        glBindVertexArray(VAO); // Bind VAO

        // Draw the Triforce
        //  - Select Triangle primitive
        //  - Specify number of indices 
        //  - Specify datatype of indices
        //  - Identify index of indices
        if (drawTriangle){ 
            glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        }

        // Create UI Window 
        ImGui::Begin("Testing a new ImGUI window!"); // ImGUI window creation
        ImGui::Text("Do you like the triangle?"); // Text that appears in the window
        ImGui::Checkbox("Draw Triangle", &drawTriangle); // Select whether to draw the triangle
        ImGui::SliderFloat("Size", &size, 0.5f, 2.0f); // Size slider that appears in the window
        ImGui::ColorEdit4("Color", color); // Fancy color editor that appears in the window
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
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(main_window); // Close window when complete
    glfwTerminate(); // Terminate glfw process
    return 0;
}

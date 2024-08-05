// GUI Headers
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Primary Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

// Shader Headers
#include "Assets/Shaders/shader.h"
#include "Assets/Buffers/VAO.h"
#include "Assets/Buffers/VBO.h"
#include "Assets/Buffers/EBO.h"

// Matrix Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Texture Headers
#include "Assets/Textures/texture.h"

// Camera Header(s)
#include "Assets/Camera/camera.h"

// C/C++ Library Headers
#include <iostream>
#include <math.h>

// Other Headers
#include "Assets/shapes.h"

// ************************
// * Define Lighting Cube *
// ************************

GLfloat light_vertices[] =
    { //     COORDINATES      //
        -0.1f, -0.1f, 0.1f,
        -0.1f, -0.1f, -0.1f,
        0.1f, -0.1f, -0.1f,
        0.1f, -0.1f, 0.1f,
        -0.1f, 0.1f, 0.1f,
        -0.1f, 0.1f, -0.1f,
        0.1f, 0.1f, -0.1f,
        0.1f, 0.1f, 0.1f};

GLuint light_indices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 4, 7,
        0, 7, 3,
        3, 7, 6,
        3, 6, 2,
        2, 6, 5,
        2, 5, 1,
        1, 5, 4,
        1, 4, 8,
        4, 5, 6,
        4, 6, 7};

int main()
{
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

    const char *win_name = "Shape Simulator"; // Window name
    // Designated fullscreen monitor
    GLFWmonitor *fullscreen = NULL; // glfwGetPrimaryMonitor();

    // Initialize window with the following parameters:
    //  width, height, name, fullscreen monitor, and context object sharing
    GLFWwindow *main_window = glfwCreateWindow(width, height, win_name, fullscreen, NULL);

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

    // *************************************************
    // * Initialize Shaders, Vertex Array, and Buffers *
    // *************************************************
    // Vertex array object MUST be created before vertex buffer object
    // Vertex buffer is a different kind of buffer than the front and back buffer

    Shader shader_program("Assets/Shaders/default.vert", "Assets/Shaders/default.frag");
    // Vertex Array Object (VAO)
    VAO vao1;
    vao1.Bind();

    // Vertex Buffer Object (VBO)
    VBO vbo1(Shapes::pyramid_vertices, sizeof(Shapes::pyramid_vertices));
    // Index Buffer Object (EBO)
    EBO ebo1(Shapes::pyramid_indices, sizeof(Shapes::pyramid_indices));

    // Link vbo layouts to corresponding vao
    // Position Coordinate layout (layout 0)
    vao1.LinkAttrib(vbo1, 0, 3, GL_FLOAT, 11 * sizeof(float), (void *)0);
    // Color layout (layout 1)
    vao1.LinkAttrib(vbo1, 1, 3, GL_FLOAT, 11 * sizeof(float), (void *)(3 * sizeof(float)));
    // Texture coordinate layout (layout 2)
    vao1.LinkAttrib(vbo1, 2, 2, GL_FLOAT, 11 * sizeof(float), (void *)(6 * sizeof(float)));
    // Normals (layout 3)
    vao1.LinkAttrib(vbo1, 3, 3, GL_FLOAT, 11 * sizeof(float), (void *)(8 * sizeof(float)));
    // Unbind vao, vbo, and ebo to avoid further modifications
    vao1.Unbind();
    vbo1.Unbind();
    ebo1.Unbind();

    // Light Shader Initialization

    Shader light_shader("Assets/Shaders/light.vert", "Assets/Shaders/light.frag");

    // Initialize and bind light VAO
    VAO light_vao;
    light_vao.Bind();

    // Initialize Light VBO and EBO
    VBO light_vbo(light_vertices, sizeof(light_vertices));
    EBO light_ebo(light_indices, sizeof(light_indices));

    // Link light VBO to corresponding VAO
    light_vao.LinkAttrib(light_vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void *)0);

    // Unbind vao, vbo, and ebo to avoid further modifications
    light_vao.Unbind();
    light_vbo.Unbind();
    light_ebo.Unbind();

    // *************************
    // * Creating Light Object *
    // *************************

    // Form color of object based on the light
    glm::vec4 light_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Initialize light object
    glm::vec3 light_pos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 light_model = glm::mat4(1.0f);
    light_model = glm::translate(light_model, light_pos);

    // Initialize pyramid object position
    glm::vec3 pyramid_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 pyramid_model = glm::mat4(1.0f);
    pyramid_model = glm::translate(pyramid_model, pyramid_pos);

    // Export light object to light shader
    light_shader.Activate();
    GLuint light_model_ID = glGetUniformLocation(light_shader.ID, "model");
    glUniformMatrix4fv(light_model_ID, 1, GL_FALSE, glm::value_ptr(light_model));
    // Export light color to light shader
    glUniform4f(glGetUniformLocation(light_shader.ID, "lightColor"), light_color.x, light_color.y, light_color.z, light_color.w);
    // Export pyramid object to shader program
    shader_program.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shader_program.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramid_model));
    // Export light color and position to shader program
    glUniform4f(glGetUniformLocation(shader_program.ID, "lightColor"), light_color.x, light_color.y, light_color.z, light_color.w);
    glUniform3f(glGetUniformLocation(shader_program.ID, "lightPos"), light_pos.x, light_pos.y, light_pos.z);

    // ********************
    // * Initialize IMGUI *
    // ********************
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Create input/output handler
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // Choose imgui style
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(main_window, true);
    ImGui_ImplOpenGL3_Init("#version 460"); // Specify initi    alization based on version of OpenGL

    // *****************************
    // * Initialize Loop Variables *
    // *****************************

    // Set Background color
    // Rebecca Purple: (0.4f, 0.2f, 0.6f, 1.0f)
    // Navy Blue: (0.07f, 0.13f, 0.17f, 1.0f)
    glm::vec4 bg_color = glm::vec4((float)(25.0 / 255.0), (float)(25.0 / 255.0), (float)(122.0 / 255.0), 1.0f);

    // Shape attributes
    // -------------------
    bool drawShape = true; // Triangle visibility
    float size = 1.0f;     // Triangle size
    // float rotation = glm::radians(0.0f); // Initial rotation value
    // bool autoRotate = false;
    // float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Triangle color

    // Ignore mouse inputs with imGUI enabled
    // if (!io.WantCaptureMouse) {
    //     // Handle input
    // }

    // Pointer to uniform variable size from shader
    GLuint sizeID = glGetUniformLocation(shader_program.ID, "size");
    // GLunit colorID = glGetUniformLocation(shader_program.ID, "color");

    // Load Texture
    Texture tex("Assets/Textures/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
    tex.texUnit(shader_program, "tex0", 0);

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    // double prevTime = glfwGetTime();

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

        shader_program.Activate(); // Activate program

        // **************
        // * Delta Time *
        // **************

        // // Rotate every 1/60th of a second
        // double currTime = glfwGetTime();
        // if (currTime - prevTime >= 1.0 / 60.0) {
        //     if (autoRotate) {
        //         rotation += 0.5f;
        //     }
        //     prevTime = currTime;
        // }

        // *******************
        // * Camera Settings *
        // *******************

        // Set input for camera
        camera.Input(main_window);

        // Update camera matrix based on view plane and FOV
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

        // Set camera position in shader
        glUniform3f(glGetUniformLocation(shader_program.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);

        // Export camera matrix to the vertex shader
        camera.Matrix(shader_program, "camMatrix");

        // Update uniforms values after activation
        glUniform1f(sizeID, size);
        // glUniform4f(colorID, color[0], color[1], color[2], color[3]);
        tex.Bind();

        vao1.Bind(); // Bind VAO

        // Draw the Triforce
        //  - Select Triangle primitive
        //  - Specify number of indices
        //  - Specify datatype of indices
        //  - Identify index of indices
        if (drawShape)
        {
            glDrawElements(GL_TRIANGLES, sizeof(Shapes::pyramid_indices) / sizeof(int), GL_UNSIGNED_INT, 0);
        }

        light_shader.Activate();
        camera.Matrix(light_shader, "camMatrix");
        light_vao.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(light_indices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Create UI Window
        ImGui::Begin("Shape Settings");                // ImGUI window creation
        ImGui::Text("Do you like this shape?");        // Text that appears in the window
        ImGui::Checkbox("Draw Shape", &drawShape);     // Select whether to draw the shape
        ImGui::SliderFloat("Size", &size, 0.5f, 2.0f); // Size slider that appears in the window
        // ImGui::Checkbox("Auto Rotate", &autoRotate); // Select whether shape should automatically rotate
        // if (!autoRotate) {
        //     ImGui::SliderAngle("Rotation", &rotation, -360.0*24, 360*24);
        // }
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

    // *****************************
    // * Delete GL objects created *
    // *****************************

    vao1.Delete();
    vbo1.Delete();
    ebo1.Delete();
    tex.Delete();
    shader_program.Delete();

    light_vao.Delete();
    light_vbo.Delete();
    light_ebo.Delete();
    light_shader.Delete();

    glfwDestroyWindow(main_window); // Close window when complete
    glfwTerminate();                // Terminate glfw process
    return 0;
}

// Backend Headers and Source Code
#include "Backend.h"

// Object Headers
#include "Mesh.h"
#include "NMRMesh.h"
#include "Model.h"

// Matrix Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// C/C++ Library Headers
#include <iostream>
#include <math.h>

// Other Headers
#include "Shapes.h"

// ************************
// * Define Lighting Cube *
// ************************

Vertex light_vertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

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
    4, 6, 7
};

// Define Shape Verts & indices
#define vertices Shapes::plane_vertices
#define indices Shapes::plane_indices

int main()
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

    // **********************
    // * Initialize Shaders *
    // **********************

    // Initialize shader program
    Shader shader_program("Assets/Shaders/default.vert", "Assets/Shaders/default.frag");
    
    // Light Shader Initialization
    Shader light_shader("Assets/Shaders/light.vert", "Assets/Shaders/light.frag");

    Shader nmr_shader("Assets/Shaders/nmr.vert", "Assets/Shaders/nmr.frag");

    // ***********************
    // * Creating NMR Object *
    // ***********************

    std::string file = "./smo.ft2";
    NMRMesh nmr(file);
    
    // Initialize NMR Object
    glm::vec4 point_color = glm::vec4(1.0, 192.0/255.0, 203.0/255.0, 1.0);
    glm::vec3 nmr_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 nmr_model = glm::mat4(1.0f);
    nmr_model = glm::translate(nmr_model, nmr_pos);

    // *************************
    // * Creating Light Object *
    // *************************

    // Form color of object based on the light
    glm::vec4 light_color = WHITE;
    // Initialize light object
    glm::vec3 light_pos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 light_model = glm::mat4(1.0f);
    light_model = glm::translate(light_model, light_pos);

    // **************************
    // * Export Data to Shaders *
    // **************************

    // Export light object to light shader
    light_shader.Activate();
    GLuint light_model_ID = glGetUniformLocation(light_shader.ID, "model");
    glUniformMatrix4fv(light_model_ID, 1, GL_FALSE, glm::value_ptr(light_model));
    // Export light color to light shader
    glUniform4f(glGetUniformLocation(light_shader.ID, "lightColor"), light_color.x, light_color.y, light_color.z, light_color.w);


    // Export shape object to shader program
    shader_program.Activate();
    // Export light color and position to shader program
    glUniform4f(glGetUniformLocation(shader_program.ID, "lightColor"), light_color.x, light_color.y, light_color.z, light_color.w);
    glUniform3f(glGetUniformLocation(shader_program.ID, "lightPos"), light_pos.x, light_pos.y, light_pos.z);

    // Export NMR object to NMR shader
    nmr_shader.Activate();
    GLuint nmr_model_ID = glGetUniformLocation(nmr_shader.ID, "model");
    glUniformMatrix4fv(nmr_model_ID, 1, GL_FALSE, glm::value_ptr(nmr_model));
    // Export point color to nmr shader
    glUniform4f(glGetUniformLocation(nmr_shader.ID, "pointColor"), point_color.x, point_color.y, point_color.z, point_color.w);

    // ********************
    // * Initialize IMGUI *
    // ********************
    ImGuiIO io = initIMGUI(main_window);

    // *****************************
    // * Initialize Loop Variables *
    // *****************************

    // Set Background color
    // Rebecca Purple: (0.4f, 0.2f, 0.6f, 1.0f)
    // Navy Blue: (0.07f, 0.13f, 0.17f, 1.0f)
    glm::vec4 bg_color = glm::vec4((float)(25.0 / 255.0), (float)(25.0 / 255.0), (float)(122.0 / 255.0), 1.0f);
    // glm::vec4 bg_color = glm::vec4((float)(28.0 / 255.0), (float)(30.0 / 255.0), (float)(41.0 / 255.0), 1.0f);
    
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

    // Initialize camera view
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    // double prevTime = glfwGetTime();

    // Enable depth testing in OpenGL rendering
    glEnable(GL_DEPTH_TEST);

    // ***************
    // * Load Models *
    // ***************

    Model model("Assets/Models/Sword/scene.gltf");
    Model light("Assets/Models/Sphere/scene.gltf");

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

        // Update uniforms values after activation
        glUniform1f(sizeID, size);
        //glUniform4f(colorID, color[0], color[1], color[2], color[3]);

        if (drawShape){
            model.Draw(shader_program, camera);
        }
        nmr.Draw(nmr_shader, camera);

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

    // *****************************
    // * Deletion and Deallocation *
    // *****************************

    closeIMGUI(); // Close ImGui and remove GL link
    shader_program.Delete();        // Delete main shader program
    light_shader.Delete();          // Delete light shader program
    glfwDestroyWindow(main_window); // Close window when complete
    glfwTerminate();                // Terminate glfw process

    return 0;
}

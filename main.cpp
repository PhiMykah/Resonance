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
#include "UI.h"

// ************************
// * Define Lighting Cube *
// ************************



// Define Shape Verts & indices
#define vertices Shapes::plane_vertices
#define indices Shapes::plane_indices
#define light_vertices Shapes::cube_vertices
#define light_indices Shapes::cube_indices

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

    // Initialize main shader program
    Shader shader_program("Assets/Shaders/default.vert", "Assets/Shaders/default.frag");
    
    // Light shader initialization
    Shader light_shader("Assets/Shaders/light.vert", "Assets/Shaders/light.frag");

    // NMR shader initialization
    Shader nmr_shader("Assets/Shaders/nmr.vert", "Assets/Shaders/nmr.frag");

    // Stencil outline shader initialization
    Shader stencil_outline("Assets/Shaders/stencil_outline.vert", "Assets/Shaders/stencil_outline.frag");

    // ***********************
    // * Creating NMR Object *
    // ***********************

    std::string nmrFile;
    std::string currFile;

    // Initialize NMR Object
    glm::vec4 point_color = glm::vec4(0.85f, 0.85f, 0.90f, 1.0f);
    glm::vec3 nmr_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 nmr_model = glm::mat4(1.0f);
    nmr_model = glm::translate(nmr_model, nmr_pos);

    // *************************
    // * Creating Light Object *
    // *************************

    Texture textures[]
    {
        Texture("Assets/Textures/Alb/planks.png", "diffuse", 0, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR),  // Load diffusion texture
        Texture("Assets/Textures/Spec/planksSpec.png", "specular", 1, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST) // Load Specular Map for Texture
    };

    // Create vectors for vertices, indices, and textures
    // Textures will be the same since light does not utilize textures
    Vertices light_verts(light_vertices, light_vertices + sizeof(light_vertices) / sizeof(Vertex));
    Indices light_ind(light_indices, light_indices + sizeof(light_indices) / sizeof(GLuint));
    Textures tex(textures, textures + sizeof(textures) / sizeof(Texture));

    Mesh light(light_verts, light_ind, tex);

    // Form color of object based on the light
    glm::vec4 light_color = WHITE;
    // Initialize light object
    glm::mat4 light_model = glm::mat4(1.0f);
    glm::vec3 light_pos;

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

    // Ignore mouse inputs with imGUI enabled
    // if (!io.WantCaptureMouse) {
    //     // Handle input
    // }
    
    // ******************************
    // * Initialize Loop Attributes *
    // ******************************

    // Set Background color
    glm::vec4 bg_color = glm::vec4((float)(25.0 / 255.0), (float)(25.0 / 255.0), (float)(122.0 / 255.0), 1.0f);
    
    // Object attributes
    bool drawShape = true; // Object visibility
    float size = 1.0f;     // Object size

    // Stencil attributes
    float outline = 0.50f; // Stencil buffer outline
    float stencil_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Stencil buffer color

    // Light Attributes 
    float light_distance = 1;
    float light_rotation = glm::radians(0.0f); // Initial rotation value

    // Pointer to uniform variable size from shader
    GLuint sizeID = glGetUniformLocation(shader_program.ID, "size");

    // Initialize camera view
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    // **********************
    // * Initialize Buffers *
    // **********************

    // Enable depth testing in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    // Enable OpenGL depth buffer function
    // Determines whether to pass or fail an incoming depth value
    // in comparison to a stored depth value
    // Options: GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL
    // GL_GEQUAL, GL_ALWAYS
    glDepthFunc(GL_LESS);

    // Enable stencil buffer
    glEnable(GL_STENCIL_TEST);
    // Define stencil operations based on whether 
    // The stencil test fails, the stencil test passes but the depth test fails,
    // and when both tests pass.
    // Possible arguments: GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, 
    // GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, and GL_INVERT
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // Enable Face Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
    
    // Disable VSYNC
    // glfwSwapInterval(0);

    // ***************
    // * Load Models *
    // ***************

    Model model("Assets/Models/Sword/scene.gltf");
    Model ground("Assets/Models/ground/scene.gltf");
    // Model trees("Assets/Models/trees/scene.gltf");

    NMRMesh * nmrMesh = (NMRMesh *) NULL;

    // *****************
    // * Frame Counter *
    // *****************

    double prevTime = 0.0;
    double currTime = 0.0;
    double timeDiff;
    unsigned int frameCounter = 0;

    // ****************** WHILE LOOP *************************

    // While loop repeats until window is told to close or user closes window
    while (!glfwWindowShouldClose(main_window))
    {
        // Update Frame Counter 

        // Get current time for frame rate
        currTime = glfwGetTime();
        // Calculate time difference
        timeDiff = currTime - prevTime;
        frameCounter++;
        // Calulate framerate every 30th of a second
        if (timeDiff >= 1.0 / 30.0){
            // Display fps based on frames within timespan
            std::string FPS = std::to_string((1.0 / timeDiff) * frameCounter);

            // Display ms based on difference relative to frames
            std::string ms = std::to_string((timeDiff / frameCounter) * 1000);

            // Generate frame message and print to window title
            std::string newTitle;
            newTitle += title;
            newTitle += " - ";
            std::string frameMsg = newTitle + FPS + "FPS / " + ms + "ms";
            glfwSetWindowTitle(main_window, frameMsg.c_str());

            // Update previous time and reset frame counter
            prevTime = currTime;
            frameCounter = 0;
        }

        // Clear back buffer and set it to color with RGBA float values
        glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);

        // Clear Color, Depth, and Stencil buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Create new UI Frame, inform opengl3 and glfw
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        shader_program.Activate(); // Activate program

        // *******************
        // * Camera Settings *
        // *******************

        // Set input for camera
        camera.Input(main_window);

        // Update camera matrix based on view plane and FOV
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

        // Update uniforms values after activation
        glUniform1f(sizeID, size);

        // *************************
        // * Light Object Settings *
        // *************************

        // Calculate light position by parametric representation of a circle
        light_pos = glm::vec3(light_distance * cos(light_rotation), 0.5f, light_distance * sin(light_rotation));
        // Send updated light position to shader program
        glUniform3f(glGetUniformLocation(shader_program.ID, "lightPos"), light_pos.x, light_pos.y, light_pos.z);

        // *******************
        // * Buffer Settings *
        // *******************

        // Enable writing to entire stencil buffer
        //            function | ref | bitwise AND mask
        glStencilFunc(GL_ALWAYS,  1,    0xFF);
        glStencilMask(0xFF);

        // *******************
        // * Main UI Drawing *
        // *******************

        // Create UI Window
        ImGui::Begin("Spectra");                // ImGUI window creation
        ImGui::Text("Do you like this shape?");        // Text that appears in the window
        ImGui::Checkbox("Draw Shape", &drawShape);     // Select whether to draw the shape
        ImGui::SliderFloat("Size", &size, 0.5f, 2.0f); // Size slider that appears in the window
        ImGui::SliderFloat("Light Distance", &light_distance, 0.5f, 5.0f); // Slider sets distance of light from center
        ImGui::SliderAngle("Light Rotation", &light_rotation, 0.0f); // Angle on circle that light object is positioned at
        // Complete UI Window definition
        ImGui::End();
        
        // ******************
        // * OpenGL Drawing *
        // ******************

        if (drawShape){
            model.Draw(shader_program, camera);
        }

        light.Draw(light_shader, camera, light_model, light_pos);

        drawFileDialog(nmrFile);

        if (!nmrFile.empty() && currFile != nmrFile) {
            if (nmrMesh != NULL) {
                delete nmrMesh;
            }
            nmrMesh = new NMRMesh(nmrFile);
            currFile = nmrFile;
        }
        if (drawShape && nmrMesh != NULL){
            nmrMesh->Draw(shader_program, camera);
        }
        
        // *******************************
        // * Post Processing & Rendering *
        // *******************************

        // Pass stencil test only when not equal to one
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        // Disable writing to stencil mask to avoid modifying afterwards
        glStencilMask(0x00);
        // Disable depth buffer to avoid modifying
        glDisable(GL_DEPTH_TEST);

        stencilUI(stencil_outline, outline, stencil_color);
        
        // Redraw objects with post-processing

        if (drawShape){
            model.Draw(stencil_outline, camera);

            if (nmrMesh != NULL) {
                nmrMesh->Draw(stencil_outline, camera);
            }
        }
        
        // Clear stencil mask and stencil test
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        // Re-enable depth buffer
        glEnable(GL_DEPTH_TEST);

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

    if (nmrMesh != NULL) {
        delete nmrMesh;
    }
    closeIMGUI(); // Close ImGui and remove GL link
    shader_program.Delete();        // Delete main shader program
    light_shader.Delete();          // Delete light shader program
    nmr_shader.Delete();            // Delete nmr shader program
    stencil_outline.Delete();       // Delete stencil shader program
    glfwDestroyWindow(main_window); // Close window when complete
    glfwTerminate();                // Terminate glfw process

    return 0;
}

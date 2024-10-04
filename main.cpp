// Backend Headers and Source Code
#include "Assets/Headers/Backend.hpp"

// Object Headers
#include "Line.hpp"
#include "Mesh.hpp"
#include "NMRMesh.hpp"
#include "Model.hpp"
#include "Cubemap.hpp"

// Matrix Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// C/C++ Library Headers
#include <iostream>
#include <math.h>
#include <filesystem>
namespace fs = std::filesystem;

// Other Headers
#include "Shapes.hpp"
#include "UI.hpp"

// Define Shape Verts & indices
#define vertices Shapes::plane_vertices
#define indices Shapes::plane_indices
#define light_vertices Shapes::cube_vertices
#define light_indices Shapes::cube_indices
#define rect Shapes::rectangle_vertices
#define skybox_vertices Shapes::skybox_vertices
#define skybox_indices Shapes::skybox_indices

int main()
{
    // Assets path
    std::string assets = fs::current_path().string() + "/Assets/";

    // GLFW parameters
    int width = 800; // Window width
    int height = 800; // Window height
    const char *title = "Resonance"; // Window name
    GLFWmonitor *fullscreen = NULL; // Use `glfwGetPrimaryMonitor()` for fullscreen

    // Collect window data
    // The variables are as follows:
    // - window width and height
    // - windowed mode's previous width and height
    // - window x position and window y position
    // - window's current title
    // - window's fullscreen monitor
    // - boolean for whether or not the window is fullscreen
    WindowData win{width, height, width, height, 0, 0, title, fullscreen, (bool) fullscreen}; 

    glfwInit();

    // Initialize glfw window with the following parameters:
    //  width, height, name, fullscreen monitor, and context object sharing
    GLFWwindow *main_window = initWindow(win.width, win.height, win.title, win.monitor);
    glfwSetWindowUserPointer(main_window, &win);

    // Since window returns null if the window fails to initialize, check for failure
    if (main_window == NULL)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate(); // Terminate glfw processes
        return -1;
    }
    glfwMakeContextCurrent(main_window); // Create window

    // Set window Callbacks
    glfwSetWindowSizeCallback(main_window, window_size_callback);
    glfwSetWindowIconifyCallback(main_window, window_iconify_callback);
    glfwSetKeyCallback(main_window, key_callback);

    // *****************
    // * Initialize GL *
    // *****************
    gladLoadGL();

    // Set openGL to render from top left corner (0,0) to bottom right corner (width, height)
    glViewport(0, 0, win.width, win.height);

    // Enable point size modification
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    // **********************
    // * Initialize Shaders *
    // **********************

    std::string shader_path = assets + "Shaders/";
    
    // Initialize main shader program
    std::string shader_name = "default";
    Shader shader_program(
        shaderFile(shader_path, shader_name, VERT).c_str(),
        shaderFile(shader_path, shader_name, FRAG).c_str(),
        shaderFile(shader_path, shader_name, GEOM).c_str()
    );
    
    // Point shader initialization
    shader_name = "points";
    Shader points_shader(
        shaderFile(shader_path, shader_name, VERT).c_str(),
        shaderFile(shader_path, shader_name, FRAG).c_str(),
        shaderFile(shader_path, shader_name, GEOM).c_str()
    );

    // Light shader initialization
    shader_name = "light";
    Shader light_shader(
        shaderFile(shader_path, shader_name, VERT).c_str(),
        shaderFile(shader_path, shader_name, FRAG).c_str(),
        shaderFile(shader_path, shader_name, GEOM).c_str()
    );

    // NMR shader initialization
    shader_name = "nmr";
    Shader nmr_shader(
        shaderFile(shader_path, shader_name, VERT).c_str(),
        shaderFile(shader_path, shader_name, FRAG).c_str(),
        shaderFile(shader_path, shader_name, GEOM).c_str()
    );

    // Stencil outline shader initialization
    shader_name = "stencil";
    Shader stencil_shader(
        shaderFile(shader_path, shader_name, VERT).c_str(),
        shaderFile(shader_path, shader_name, FRAG).c_str(),
        shaderFile(shader_path, shader_name, GEOM).c_str()
    );

    // Skybox shader initialization
    shader_name = "skybox";
    Shader skybox_shader(
        shaderFile(shader_path, shader_name, VERT).c_str(),
        shaderFile(shader_path, shader_name, FRAG).c_str(),
        shaderFile(shader_path, shader_name, GEOM).c_str()
    );

    // Projection shader initialization
    shader_name = "projection";
    Shader projection_shader(
        shaderFile(shader_path, shader_name, VERT).c_str(),
        shaderFile(shader_path, shader_name, FRAG).c_str(),
        shaderFile(shader_path, shader_name, GEOM).c_str()
    );

    // Normals shader initialization
    shader_name = "normals";
    Shader normals_shader(
        shaderFile(shader_path, shader_name, VERT).c_str(),
        shaderFile(shader_path, shader_name, FRAG).c_str(),
        shaderFile(shader_path, shader_name, GEOM).c_str()
    );

    // Line shader initialization
    shader_name = "lines";
    Shader line_shader(
        shaderFile(shader_path, shader_name, VERT).c_str(),
        shaderFile(shader_path, shader_name, FRAG).c_str(),
        shaderFile(shader_path, shader_name, GEOM).c_str()
    );

    // ***********************
    // * Creating NMR Object *
    // ***********************

    std::string nmrFile;
    std::string currFile;

    // Initialize NMR Object
    glm::vec4 point_color = glm::vec4(0.85f, 0.85f, 0.90f, 1.0f);
    glm::vec3 nmr_pos = ZEROS;
    glm::mat4 nmr_model = MAT_IDENTITY;
    nmr_model = glm::translate(nmr_model, nmr_pos);

    // *************************
    // * Creating Light Object *
    // *************************

    Texture textures[]
    {
        Texture((assets + "Textures/Alb/planks.png").c_str(), "diffuse", 0, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR),  // Load diffusion texture
        Texture((assets + "Textures/Spec/planksSpec.png").c_str(), "specular", 1, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST) // Load Specular Map for Texture
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
    glm::mat4 light_model = MAT_IDENTITY;
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

    // Export skybox texture to skybox shader
    skybox_shader.Activate();
    glUniform1i(glGetUniformLocation(skybox_shader.ID, "skybox"), 0);

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
    
    // General attributes
    glm::quat rot = QUAT_IDENTITY;
    glm::vec3 eulerRotation = glm::eulerAngles(rot);

    // Object attributes
    bool drawShape = true; // Object visibility
    bool drawBoundingBox = true;
    // float objSize = 0.250;     // Object size
    // glm::vec3 objPos = glm::vec3(0.5f, 0.0f, 1.0f);
    // glm::vec3 objScale = ONES;

    // NMR attributes
    bool showNMR = false;
    bool drawPoints = false;
    float pointSize = 1.0;
    float nmrSize = 1.0;
    glm::mat4 nmrMat = MAT_IDENTITY;
    glm::vec3 nmrPos = ZEROS;
    glm::vec3 nmrScale = ONES;

    // Gizmo attributes
    bool showGizmo = true;

    // Normals display attributes
    bool showNormals = false;
    float normalLength = 0.01f;

    // Bounding box attributes
    glm::vec3 bbPos = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 bbScale = glm::vec3(2.0);

    // Stencil attributes
    float outline = 0.50f; // Stencil buffer outline
    float stencil_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Stencil buffer color

    // Light Attributes 
    float light_distance = 0.8;
    float light_rotation = glm::radians(0.0f); // Initial rotation value

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
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CCW);
    
    // Enable/Disable VSYNC (1 or 0)
    glfwSwapInterval(1);

    /* 
       Choose Color + Alpha blending settings
       --------------------------------------
       Parameters are sfactor and dfactor,
       Where sfactor determines the formula for the source 
       color (fragment shader) and the dfactor determines
       the formula for the destination color (color buffer)
       Possible Constants: (The initial value is GL_ZERO)
       GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR,
       GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, 
       GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA,
       GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_ALPHA,
       GL_ONE_MINUS_CONSTANT_ALPHA. 
    */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /* 
       glBlendEquation is also an option to customize the function.
       Arguments include GL_FUNC_ADD, GL_FUNC_SUBTRACT,
       GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX

       glBlendFuncSeparate() Also allows function modification
       But with RGB and Alpha separate
    */

    // ***************
    // * Load Assets *
    // ***************

    // Model model((assets + "Models/Sword/scene.gltf").c_str());
    // Model ground((assets + "Models/ground/scene.gltf").c_str());
    // Model trees("Assets/Models/trees/scene.gltf");

    std::vector<LineVertex> axis_points;

    axis_points.push_back({
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3( 0.0f,  0.0f,  0.0f),
    });
    axis_points.push_back({
        glm::vec3( 1.0f, -1.0f, -1.0f),
        glm::vec3( 0.0f,  0.0f,  0.0f)
    });
    axis_points.push_back({
        glm::vec3(-1.0f, 1.0f, -1.0f),
        glm::vec3(0.0f,  0.0f,  0.0f),
    });
    axis_points.push_back({
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(0.0f,  0.0f,  0.0f),
    });

    glLineWidth(4.0f);

    Indices line_points = {0, 1, 0, 2, 0, 3};
    Line axis_lines(axis_points, line_points);

    NMRMesh * nmrMesh = (NMRMesh *) NULL;

    // Initialize camera view
    Camera camera(win.width, win.height, glm::vec3(0.0f, 0.0f, 4.0f));
    // Initialize camera variables 
    float FOVdeg = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    Cubemap boundingBox((assets + "Textures/Skybox/SolidColor/").c_str(), PNG);

    boundingBox.BindTextures();

    // *****************
    // * Frame Counter *
    // *****************

    double prevTime = 0.0;
    double currTime = 0.0;
    double deltaTime;
    unsigned int frameCounter = 0;

    // *********************
    // * Skybox Generation *
    // *********************

    // Define path
    std::string skybox_path = assets + "Textures/Skybox/AutumnFieldPureSky/";
    FileType format = PNG;

    Cubemap skybox(skybox_path.c_str(), format);

    skybox.BindTextures();

    // ****************** WHILE LOOP *************************

    // While loop repeats until window is told to close or user closes window
    while (!glfwWindowShouldClose(main_window))
    {
        // Update window size
        glfwGetWindowSize(main_window, &win.width, &win.height);

        // Update Frame Counter 

        // Get current time for frame rate
        currTime = glfwGetTime();
        // Calculate time difference
        deltaTime = currTime - prevTime;
        frameCounter++;
        // Calulate framerate every 30th of a second
        if (deltaTime >= 1.0 / 30.0){
            // Display fps based on frames within timespan
            std::string FPS = std::to_string((1.0 / deltaTime) * frameCounter);

            // Display ms based on difference relative to frames
            std::string ms = std::to_string((deltaTime / frameCounter) * 1000);

            // Generate frame message and print to window title
            std::string newTitle;
            newTitle += title;
            newTitle += " - ";
            std::string frameMsg = newTitle + FPS + "FPS / " + ms + "ms";
            glfwSetWindowTitle(main_window, frameMsg.c_str());
            win.title = frameMsg.c_str();

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
        ImGuizmo::BeginFrame();
        shader_program.Activate(); // Activate program

        // *******************
        // * Camera Settings *
        // *******************

        // Set input for camera
        camera.Input(main_window);

        // Update camera matrix based on view plane and FOV
        camera.UpdateMatrix(win.width, win.height, FOVdeg, nearPlane, farPlane);

        // *************************
        // * Light Object Settings *
        // *************************

        // Calculate light position by parametric representation of a circle
        light_pos = glm::vec3(light_distance * cos(light_rotation), 0.5f, light_distance * sin(light_rotation));
        // Send updated light position to shader program
        glUniform3f(glGetUniformLocation(shader_program.ID, "lightPos"), light_pos.x, light_pos.y, light_pos.z);

        // **************************
        // * Normal Vector Settings *
        // **************************

        normals_shader.Activate();
        glUniform1f(glGetUniformLocation(normals_shader.ID, "hairLength"), normalLength);

        // ******************
        // * Point Settings *
        // ******************

        points_shader.Activate();
        glUniform1f(glGetUniformLocation(points_shader.ID, "pointSize"), pointSize);

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

        if (showNMR)
        {
            // Create UI Window
            spectraUI(
                &drawShape, &drawBoundingBox, drawPoints, &pointSize, &nmrSize, &showNormals, 
                &normalLength, &light_distance, &light_rotation);
        }

        // ******************
        // * OpenGL Drawing *
        // ******************

        glDisable(GL_STENCIL_TEST);
        // Draw skybox
        skybox.DrawSkybox(skybox_shader, camera, win.width, win.height);
        glEnable(GL_STENCIL_TEST);

        // if (drawShape){
        //     model.Draw(shader_program, camera, objPos, rot, objSize * objScale);
        // }

        light.Draw(light_shader, camera, light_model, nmrPos + light_pos);

        drawMainMenu(nmrFile, main_window);

        // Create new NMRMesh if necessary
        if (!nmrFile.empty() && currFile != nmrFile) {
            if (nmrMesh != NULL) {
                delete nmrMesh;
            }
            nmrMesh = new NMRMesh(nmrFile);
            currFile = nmrFile;
            showNMR = true;
            // Reset attributes
            nmrPos = ZEROS;
            nmrScale = ONES;
            rot = QUAT_IDENTITY;
            eulerRotation = glm::eulerAngles(rot);
        }

        // Draw NMRMesh
        if ((showNMR && drawShape) && nmrMesh != NULL){

            ImGui::Begin("Gizmo");
            ImGui::Checkbox("Show Gizmo", &showGizmo);
            drawCubeView(camera, win, FOVdeg, nearPlane, farPlane);
            if (showGizmo){
                EditTransform(camera, nmrPos, rot, eulerRotation, nmrScale, win, 45.0f, 0.1f, 100.0f);
            };
            ImGui::End();
            
            if (drawPoints){
                nmrMesh->SetPrimative(GL_POINTS);
                nmrMesh->Draw(points_shader, camera, nmrMat, nmrPos, rot, nmrSize * nmrScale);
            } else {
                nmrMesh->SetPrimative(GL_TRIANGLES);
                nmrMesh->Draw(shader_program, camera, nmrMat, nmrPos, rot, nmrSize * nmrScale);
            }
            if (showNormals) {
                nmrMesh->Draw(normals_shader, camera, nmrMat, nmrPos, rot, nmrSize * nmrScale);
            }
            axis_lines.Draw(line_shader, camera);
        }

        // Draw bounding box with inverted culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
        if (drawBoundingBox) {
            boundingBox.Draw(projection_shader, camera, MAT_IDENTITY, bbPos, rot, nmrSize * bbScale);
        }
        glDisable(GL_CULL_FACE);

        // *******************************
        // * Post Processing & Rendering *
        // *******************************

        // Pass stencil test only when not equal to one
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        // Disable writing to stencil mask to avoid modifying afterwards
        glStencilMask(0x00);
        // Disable depth buffer to avoid modifying
        glDisable(GL_DEPTH_TEST);
        // Enable rgb + alpha blending
        glEnable(GL_BLEND);

        if (showNMR) {
            stencilUI(stencil_shader, outline, stencil_color);
        }
        
        // Redraw objects with post-processing

        if (drawShape){
            // model.Draw(stencil_shader, camera, objPos, rot, objSize * objScale);
            if (nmrMesh != NULL) {
                nmrMesh->Draw(stencil_shader, camera, nmrMat, nmrPos, rot, nmrSize * nmrScale);
            }
        }
        
        // Disable RGB + Alpha blending
        glDisable(GL_BLEND);
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
    stencil_shader.Delete();       // Delete stencil shader program
    skybox_shader.Delete();         // Delete skybox shader program
    projection_shader.Delete();     // Delete projection shader program
    normals_shader.Delete();        // Delete normal shader program
    line_shader.Delete();           // Delete line shader program
    glfwDestroyWindow(main_window); // Close window when complete
    glfwTerminate();                // Terminate glfw process

    return 0;
}

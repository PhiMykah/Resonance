// Backend Headers and Source Code
#include "Assets/Headers/Backend.hpp"

// Object Headers
#include "Line.hpp"
#include "Mesh.hpp"
#include "NMRMesh.hpp"
#include "Model.hpp"
#include "Cubemap.hpp"
#include "Shader.hpp"
#include "FBO.hpp"
#include "Type.hpp"
#include "Light.hpp"

// Matrix Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// C/C++ Library Headers
#include <iostream>
#include <math.h>
#include <filesystem>
namespace fs = std::filesystem;

// Define Shape Verts & indices
#define vertices Shapes::plane_vertices
#define indices Shapes::plane_indices
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
    int prevWidth = width;
    int prevHeight = height;

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

    std::vector<std::string> shader_list = {
        "default", "default2d", "points", "point2d",
        "light", "nmr", "stencil", "skybox", "projection",
        "normals", "lines", "selection", "text", 
    };

    std::map<std::string, Shader> shaders;
    
    // Initialize all shader programs
    initializeShaders(shaders, shader_path, shader_list);

    // ***********************
    // * Creating NMR Object *
    // ***********************

    std::map<std::string, void *> nmrMeshes;
    // std::string nmrFile;
    std::string currFile;

    // Initialize NMR Object
    glm::vec4 point_color = glm::vec4(0.85f, 0.85f, 0.90f, 1.0f);
    glm::vec3 nmr_pos = ZEROS;
    glm::mat4 nmr_model = MAT_IDENTITY;
    nmr_model = glm::translate(nmr_model, nmr_pos);

    // **************************
    // * Export Data to Shaders *
    // **************************

    // Export NMR object to NMR shader
    shaders["nmr"].Activate();
    shaders["nmr"].setMat4("model", nmr_model);
    // Export point color to nmr shader
    shaders["nmr"].setVec4("pointColor", point_color);

    // Export skybox texture to skybox shader
    shaders["skybox"].Activate();
    shaders["skybox"].setInt("skybox", 0);

    // ********************
    // * Initialize IMGUI *
    // ********************
    initIMGUI(main_window);
    ImGuiIO io = ImGui::GetIO();

    // Ignore mouse inputs with imGUI enabled
    // if (!io.WantCaptureMouse) {
    //     // Handle input
    // }
    
    // ******************************
    // * Initialize Loop Attributes *
    // ******************************

    // Set Background color
    glm::vec4 bg_color = glm::vec4((float)(25.0 / 255.0), (float)(25.0 / 255.0), (float)(122.0 / 255.0), 1.0f);

    // Set everything with render settings
    glRenderSettings();

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

    Indices line_points = {0, 1, 0, 2, 0, 3};
    Line axis_lines(axis_points, line_points);

    NMRMesh * nmrMesh = (NMRMesh *) NULL;
    NMRMesh * currMesh = (NMRMesh *) NULL;

    // Initialize camera view
    Camera camera(win.width, win.height, glm::vec3(0.0f, 0.0f, 4.0f));

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

    // **************************
    // * Selection Frame Buffer *
    // **************************

    SelectionFBO selection;
    selection.Init(width, height);
    
    // *************
    // * Text Init *
    // *************

    Type t("Assets/Fonts/Arial.ttf");

    glm::vec4 text_pos = glm::vec4(0.0f);
    glm::vec4 prev_pos = text_pos;
    glm::vec2 center_point = glm::vec2(0.0f);

    // ******************
    // * Light Vertices *
    // ******************

    std::vector<Light*> lights;

    // ****************** WHILE LOOP *************************

    // While loop repeats until window is told to close or user closes window
    while (!glfwWindowShouldClose(main_window))
    {   
        // Update window size
        glfwGetWindowSize(main_window, &win.width, &win.height);

        if (win.width != prevWidth || win.height != prevHeight) {
            selection.Resize(win.width, win.height);
        }

        if (win.width && win.height)
            Type::SetProjection(0, static_cast<float>(win.width), 0, static_cast<float>(win.height));
        prevWidth = win.width;
        prevHeight = win.height;

        shaders["text"].Activate();
        shaders["text"].setMat4("projection", Type::GetProjection());

        // ************************
        // * Update Frame Counter *
        // ************************

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
        shaders["default"].Activate(); // Activate program

        // *******************
        // * Camera Settings *
        // *******************

        // Set input for camera
        camera.Input(main_window);

        // Update camera matrix based on view plane and FOV
        if (win.width > 0 && win.height > 0) {
            camera.UpdateMatrix(win.width, win.height);
        }

        // *******************
        // * Buffer Settings *
        // *******************

        // Enable writing to entire stencil buffer
        //            function | ref | bitwise AND mask
        glStencilFunc(GL_ALWAYS,  1,    0xFF);
        glStencilMask(0xFF);

        // ******************
        // * OpenGL Drawing *
        // ******************

        glDisable(GL_STENCIL_TEST);
        // Draw skybox
        if (win.width > 0 && win.height > 0)
            skybox.DrawSkybox(shaders["skybox"], camera, win.width, win.height);  
        glEnable(GL_STENCIL_TEST);

        DrawMainMenu(nmrMeshes, lights, currFile, main_window);

        for (auto light: lights) {
            light->Display(win, camera, shaders);
        }

        // Create new NMRMesh if necessary
        if ((nmrMeshes.find(currFile) != nmrMeshes.end()) && (!currFile.empty())) {
            if (nmrMeshes.at(currFile) == NULL) {
                nmrMesh = new NMRMesh(currFile);

                nmrMeshes[currFile] = static_cast<void*>(nmrMesh);

                nmrMesh->resetAttributes();
            }
        }
        
        selection.SelectMesh(shaders["selection"], camera, nmrMeshes);

        for (auto const& [key, val] : nmrMeshes) {
            currMesh = static_cast<NMRMesh *>(val);
            if (currMesh != NULL) {
                currMesh->updateUniforms(shaders);
                currMesh->Display(win, camera, shaders);

                glLineWidth(4.0f);
                axis_lines.Draw(shaders["lines"], camera);
                glLineWidth(1.0f);
            }
        }
        
        MeshList(nmrMeshes);
        
        io = ImGui::GetIO();
        // Mouse selection 
        // Ensure mouse is not over an ImGui window
        if (!io.WantCaptureMouse) {
            if (glfwGetMouseButton(main_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                double x, y;
                glfwGetCursorPos(main_window, &x, &y);
                FBO::Pixel selected_pixel = selection.ReadPixel(static_cast<GLuint>(x),  static_cast<GLuint>(abs(win.height - y)));
                selection.currSel = selected_pixel.objID; 
                NMRMesh::selID = selected_pixel.objID;
            }
        }

        // ******************
        // * Text Rendering *
        // ******************

        ActivateTextSettings();

        if (nmrMeshes.empty() && lights.empty()) {
            float text_x = static_cast<float>(win.width) / 2.0f;
            float text_y = static_cast<float>(win.height) / 2.0f;
            text_pos = t.RenderText(shaders["text"], "Welcome!", 
                         text_x, 
                         text_y + 50, 
                         1.0f, glm::vec3(0.0, 0.0, 0.0), true);
            t.RenderText(shaders["text"], "Use File->Open or Ctrl+O",
                         text_x,
                         text_y,
                         1.0f, glm::vec3(0.0, 0.0, 0.0), true);
            t.RenderText(shaders["text"], "to open a file.",
                         text_x,
                         text_y - 50,
                         1.0f, glm::vec3(0.0, 0.0, 0.0), true);
        
            center_point = glm::vec2(text_pos.x + text_pos.y / 2.0f, text_pos.z + text_pos.w / 2.0f);

            #ifdef DEBUG
                        t.RenderCenter(shaders["point2d"], center_point, glm::vec3(0.0));
                        if ((text_pos != prev_pos)) {
                            printf("Text Center: (%.3f, %.3f)\n", center_point.x, center_point.y);
                            printf("(%.3f + %.3f, %.3f + %.3f)\n", text_pos.x, text_pos.y / 2, text_pos.z, text_pos.w / 2);
                        }
                        prev_pos = text_pos;
            #endif // DEBUG
        }
        
        DeactivateTextSettings();

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
    // Delete all shader programs
    for (auto & [name, shader] : shaders) {
        shader.Delete();
    }
    selection.Delete();
    glfwDestroyWindow(main_window); // Close window when complete
    glfwTerminate();                // Terminate glfw process

    return 0;
}

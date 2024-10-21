#include "Light.hpp"

unsigned int Light::selID = std::numeric_limits<unsigned int>::max();

Light::Light()
{
    Init(0, LightType::POINT);
}

Light::Light(unsigned int ID, LightType caster)
{
    Init(ID, caster);
}

void Light::Init(unsigned int ID, LightType caster)
{
    Texture textures[] = 
    {
        Texture("Assets/Textures/Alb/planks.png", "diffuse", 0, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR),  // Load diffusion texture
        Texture("Assets/Textures/Spec/planksSpec.png", "specular", 1, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST) // Load Specular Map for Texture
    };

    Vertices light_verts(Shapes::cube_vertices, 
                         Shapes::cube_vertices + sizeof(Shapes::cube_vertices) / sizeof(Vertex));
    Indices light_ind(Shapes::cube_indices,
                      Shapes::cube_indices + sizeof(Shapes::cube_indices) / sizeof(GLuint));
    Textures tex(textures, textures + sizeof(textures) / sizeof(Texture));

    initMesh(light_verts, light_ind, tex);

    Light::caster = caster;
    Light::ID = ID;
}

void Light::UpdateUniforms(Shader &shader) {

    pos = glm::vec3(distance * cos(rotation), 0.5, distance * sin(rotation));

    shader.Activate();

    std::string lightPos;
    std::string lightColor;
    std::string lightAmbient;
    std::string lightDiffuse;
    std::string lightSpecular;
    std::string lightAttr1;
    std::string lightAttr2;
    std::string lightAttr3;

    switch (caster)
    {
    case LightType::DIRECTIONAL:
        lightPos = "dirLight.direction";
        lightAmbient = "dirLight.ambient";
        lightDiffuse = "dirLight.diffuse";
        lightSpecular = "dirLight.specular";
        lightColor = "dirLight.color";
        break;
    case LightType::SPOT:
        lightPos = "spotLight.direction";
        lightAmbient = "spotLight.ambient";
        lightDiffuse = "spotLight.diffuse";
        lightSpecular = "spotLight.specular";
        lightColor = "spotLight.color";
        break;
    case LightType::POINT:
    default:
        lightPos = "pointLights[" + std::to_string(ID) + "].position";
        lightAmbient = "pointLights[" + std::to_string(ID) + "].ambient";
        lightDiffuse = "pointLights[" + std::to_string(ID) + "].diffuse";
        lightSpecular = "pointLights[" + std::to_string(ID) + "].specular";
        lightColor = "pointLights[" + std::to_string(ID) + "].color";
        lightAttr1 = "pointLights[" + std::to_string(ID) + "].constant";
        lightAttr2 = "pointLights[" + std::to_string(ID) + "].linear";
        lightAttr3 = "pointLights[" + std::to_string(ID) + "].quadratic";
        break;
    }

    // Send updated light position to shader program
    shader.setVec3(lightPos, pos);
    // Export light color and position to shader program
    shader.setVec3(lightColor, glm::vec3(color));
    // Export ambient to shader program
    shader.setVec3(lightAmbient, ambient);
    // Export diffuse to shader program
    shader.setVec3(lightDiffuse, diffuse);
    // Export specular to shader program
    shader.setVec3(lightSpecular, specular);

    if ((caster == LightType::POINT) || (caster == LightType::SPOT)) {
        shader.setFloat(lightAttr1, constant);
        shader.setFloat(lightAttr2, linear);
        shader.setFloat(lightAttr3, quadratic);
    }
}

void Light::Display(WindowData &win, Camera &camera, Shaders &shaders)
{
    UpdateUniforms(shaders["default"]);
    
    if (selID == ID) {
        DisplayUI(win, camera);
    }

    // Export light object to light shader
    shaders["light"].Activate();
    // Export light color to light shader
    shaders["light"].setVec4("lightColor", color);
    Draw(shaders["light"], camera, mat, pos, rot, scale);

    if (selID == ID){
        DisplayStencil(shaders, camera);
    }
}

void Light::DisplayUI(WindowData &win, Camera &camera)
{
    if (ImGui::Begin("Light Settings")) {                                       // Text for light settings
        ImGui::SliderFloat("Light Distance", &distance, 0.5f, 5.0f);            // Slider sets distance of light from center
        ImGui::SliderAngle("Light Rotation", &rotation, 0.0f);                  // Angle on circle that light object is positioned at

        // EditTransform(camera, win);
    }
    // float radians = glm::radians(rotation);
    // glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 1.0f, 0.0f));
    // // Calculate light position by parametric representation of a circle
    // // pos = glm::vec3(distance * cos(rotation), 0.5, distance * sin(rotation));
    // glm::vec4 newPos = rotationMatrix * glm::vec4(g, 1.0f);
    // pos = glm::vec3(newPos) + pos;
    ImGui::End();   
}

void Light::DisplayStencil(Shaders &shaders, Camera & camera)
{
    shaders["stencil"].Activate();
    shaders["stencil"].setVec4("color", BLACK);
    shaders["stencil"].setFloat("outlining", 10.0f);
    shaders["stencil"].setMat4("gTranslation", glm::translate(MAT_IDENTITY, pos));
    shaders["stencil"].setMat4("gRotation", glm::mat4_cast(rot));
    shaders["stencil"].setMat4("gScale", glm::scale(MAT_IDENTITY, scale));
    
    // Pass stencil test only when not equal to one
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // Disable writing to stencil mask to avoid modifying afterwards
    glStencilMask(0x00);
    // Disable depth buffer to avoid modifying
    glDisable(GL_DEPTH_TEST);
    // Enable rgb + alpha blending
    glEnable(GL_BLEND);
    
    // Redraw objects with post-processing
    if (drawLight){
        Draw(shaders["stencil"], camera, mat, pos, rot, scale);
    }

    // Disable RGB + Alpha blending
    glDisable(GL_BLEND);
    // Clear stencil mask and stencil test
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    // Re-enable depth buffer
    glEnable(GL_DEPTH_TEST);
}

void Light::EditTransform(const Camera &camera, WindowData win)
{
    eulerRotation = glm::eulerAngles(rot);
    glm::mat4 modelMatrix = glm::translate(MAT_IDENTITY, pos) * glm::mat4_cast(rot) * glm::scale(MAT_IDENTITY, scale);

    float matrix[16];

    memcpy(matrix, glm::value_ptr(modelMatrix), sizeof(float) * 16);

    rot = glm::quat(eulerRotation);
    modelMatrix = glm::translate(MAT_IDENTITY, pos) * glm::mat4_cast(rot) * glm::scale(MAT_IDENTITY, scale);
    memcpy(matrix, glm::value_ptr(modelMatrix), sizeof(float) * 16);

    ImGuiIO& io = ImGui::GetIO();
    
    glm::mat4 view = MAT_IDENTITY;
    glm::mat4 projection = MAT_IDENTITY;

    view = glm::lookAt(camera.position, camera.position + camera.orientation, camera.up);
    projection = glm::perspective(glm::radians(camera.FOVdeg), (float)(float(win.width)/(float)(win.height)), camera.nearPlane, camera.farPlane);

    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(
        (float *)glm::value_ptr(view), (float *)glm::value_ptr(projection), 
        ImGuizmo::TRANSLATE, ImGuizmo::WORLD, matrix
    );
    
    // Decompose the matrix to pos, rot, scale
    glm::mat4 newMatrix = glm::make_mat4(matrix);
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(newMatrix, scale, rot, pos, skew, perspective);
}

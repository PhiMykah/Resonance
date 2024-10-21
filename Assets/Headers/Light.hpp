#ifndef LIGHT_HEADER_H
#define LIGHT_HEADER_H

#define MAX_LIGHTS 4

#include "Backend.hpp"
#include "Mesh.hpp"
#include "Shapes.hpp"

enum LightType
{
    POINT,
    DIRECTIONAL,
    SPOT
};

class Light : public Mesh
{
    public:
        Light(/* args */);
        Light(unsigned int ID, LightType light);
        void Init(unsigned int ID, LightType light);

        void UpdateUniforms(Shader &shader);

        // Display object instance
        void Display(WindowData &win, Camera & camera, Shaders &shaders);

        // Display all UI elements
        void DisplayUI(WindowData &win, Camera & camera);

        void DisplayStencil(Shaders &shaders, Camera & camera);

        void EditTransform(const Camera &camera, WindowData win);

        static unsigned int selID;
    private:
        bool drawLight = true;
        LightType caster;
        unsigned int ID;

        glm::mat4 mat = MAT_IDENTITY;
        glm::vec3 pos = ZEROS;
        glm::quat rot = QUAT_IDENTITY;
        glm::vec3 scale = ONES;
        glm::vec3 eulerRotation;

        // Light attributes
        glm::vec4 color = WHITE;
        glm::mat4 model = MAT_IDENTITY;
        float distance = 0.8f;
        float rotation = 0.0f;

        // Point attributes
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;  
        glm::vec3 ambient = glm::vec3(0.5f);
        glm::vec3 diffuse = ONES;
        glm::vec3 specular = ONES;
};

#endif // !LIGHT_HEADER_H
#ifndef UI_HEADER_H
#define UI_HEADER_H

#include "Backend.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Constants.hpp"

static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f };

void drawMainMenu(std::string& file, GLFWwindow * window);

void EditTransform(
    const Camera& camera, glm::vec3& pos, 
    glm::quat& rot, glm::vec3& eulerAngles, glm::vec3& scale, 
    WindowData win
    );

void drawCubeView(Camera & camera, WindowData win);

void stencilUI(Shader & shader, float & outline, float color[4]);

void spectraUI(
        bool * drawShape, bool * drawBB, bool &drawPoints, float * pointSize,
        float * nmrSize, bool * showNormals, float * normalLength, 
        float * light_distance, float * light_rotation
    );

#endif // !UI_HEADER_H
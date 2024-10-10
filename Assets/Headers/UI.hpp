#ifndef UI_HEADER_H
#define UI_HEADER_H

#include "Backend.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Constants.hpp"
#include <filesystem>

namespace fs = std::filesystem;

static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f };

void drawMainMenu(std::map<std::string, void *>& nmrFiles, std::string & currFile, GLFWwindow * window);

void EditTransform(
    const Camera& camera, glm::vec3& pos, 
    glm::quat& rot, glm::vec3& eulerAngles, glm::vec3& scale, 
    WindowData win
    );

void drawCubeView(Camera & camera, WindowData win);

#endif // !UI_HEADER_H
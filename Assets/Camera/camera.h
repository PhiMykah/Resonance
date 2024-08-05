#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "../Shaders/shaderClass.h"

#define DEFAULT_SPEED 0.1f;

class Camera
{
    public:
        // Position of camera object
        glm::vec3 position;

        // Orientation in which the camera is facing (forward)
        glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);

        // Camera up direction (which way is up for the camera)
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 cameraMatrix = glm::mat4(1.0f);

        // Prevents the camera from jumping around when first clicking left click
        bool firstClick = true;

        // Width and height of camera view
        int width, height;

        // Speed of the camera's movement
        float speed = DEFAULT_SPEED;

        // Camera's movement sensitivity
        float sensitivity = 100.0f;

        // Main camera constructor
        Camera(int width, int height, glm::vec3 position);

        // Update the view and projection matrices of the camera
        void UpdateMatrix(float FOVdeg, float nearPlane, float farPlane);

        // Export the camera matrix to the vertex Shader
        void Matrix(Shader& shader, const char* uniform);

        // Camera input handler
        void Input(GLFWwindow * window);
};
#endif // !CAMERA_CLASS_H

#include "Camera.hpp"
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

/*
Main initialization funciton for camera object

Parameters
----------
width : int
    Width of camera view

height : int
    Height of camera view

position : glm::vec3
    Position of camera in world space

Returns
-------
Camera object
*/
Camera::Camera(int width, int height, glm::vec3 position){
    Camera::width = width;
    Camera::height = height;

    Camera::originalWidth = width;
    Camera::originalHeight = height;

    Camera::position = position;
}

/*

Parameters
----------
FOVdeg : float
    FOV of camera

nearPlane : float
    Near cut-off plane for which no objects are rendered

farPlane : float
    Far cut-off plane for which no objects are rendered
*/
void Camera::UpdateMatrix(int width, int height, float FOVdeg, float nearPlane, float farPlane){
    Camera::width = width;
    Camera::height = height;

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    view = glm::lookAt(position, position + orientation, up);
    projection = glm::perspective(glm::radians(FOVdeg), (float)(float(width)/(float)(height)), nearPlane, farPlane);

    cameraMatrix = projection * view;
}


/* Export camera matrix to shader 

Parameters
----------
shader : Shader&
    Shader to send the uniform matrix to

uniform : const char *
    Target uniform in shader to send camera view matrix

Returns
-------
None
*/
void Camera::Matrix(Shader& shader, const char* uniform){
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

/* Handle the camera controls for the view

Parameters
----------
window : GLFWwindow*
    Pointer to target window for which the camera will be controlled

Returns
-------
None
*/
void Camera::Input(GLFWwindow * window, double deltaTime){
    deltaSpeed = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        
        position += deltaSpeed * orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        position += deltaSpeed * -glm::normalize(glm::cross(orientation, up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        position += deltaSpeed * -orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        position += deltaSpeed * glm::normalize(glm::cross(orientation, up));
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            position += deltaSpeed * up;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
            position += deltaSpeed * -up;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
       speed = 4*DEFAULT_SPEED;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
       speed = DEFAULT_SPEED;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, float(width) / 2, float(height) / 2);
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalize and shift the coordinates of the cursor such that they begin in the middle of the screen
		
        // Calculate rot x and rot y normalization
        float norm_mouse_y = (float)(floor(mouseY) - (float(height) / 2));
        float norm_mouse_x = (float)(floor(mouseX) - (float(width) / 2));

        // Fix odd number aspect ratio from leading to movement
        if (abs(norm_mouse_y) == 0.5)  norm_mouse_y = 0.0;
        if (abs(norm_mouse_x) == 0.5) norm_mouse_x = 0.0;

        // Calculate rotX and rotY
		float rotX = sensitivity * (norm_mouse_y / height);
		float rotY = sensitivity * (norm_mouse_x / width);

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (std::abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		orientation = glm::rotate(orientation, glm::radians(-rotY), up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (double(width) / 2), (double(height) / 2));
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE){
        // Unhides cursor since camera is not looking around
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        // Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
    }
}
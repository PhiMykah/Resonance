#ifndef CONSTANTS_H
#define CONSTANTS_H 

#include <glm/glm.hpp>

#define WHITE glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define BLACK glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)

#define ONES glm::vec3(1.0f)
#define ZEROS glm::vec3(0.0f)
#define QUAT_IDENTITY glm::quat(1.0f, 0.0f, 0.0f, 0.0f)
#define MAT_IDENTITY glm::mat4(1.0f)

#endif // !CONSTANTS_H
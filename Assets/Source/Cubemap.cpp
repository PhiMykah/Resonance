#include "Cubemap.h"

// ******************
// * Define Cubemap *
// ******************
float vertices[] =
{
    //   Coordinates
    -1.0f, -1.0f,  1.0f,//        7--------6
    1.0f, -1.0f,  1.0f,//       /|       /|
    1.0f, -1.0f, -1.0f,//      4--------5 |
    -1.0f, -1.0f, -1.0f,//      | |      | |
    -1.0f,  1.0f,  1.0f,//      | 3------|-2
    1.0f,  1.0f,  1.0f,//      |/       |/
    1.0f,  1.0f, -1.0f,//      0--------1
    -1.0f,  1.0f, -1.0f
};

unsigned int indices[] =
{
    // Right
    1, 2, 6,
    6, 5, 1,
    // Left
    0, 4, 7,
    7, 3, 0,
    // Top
    4, 5, 6,
    6, 7, 4,
    // Bottom
    0, 3, 2,
    2, 1, 0,
    // Back
    0, 1, 5,
    5, 4, 0,
    // Front
    3, 7, 6,
    6, 2, 3
};

Cubemap::Cubemap(const char* cubeDir, FileType format){

    // Create VAO, VBO, and EBO for the skybox
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

    // Bind VAO, VBO, and EBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    // Define vertex attribute pointer and unbind after processing
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::string formatStr = ".png";
    if (format == JPG){
        formatStr = ".jpg";
    }

    // Load cube map textures
    // Textures must be in this order

	faces[0] = std::string(cubeDir) + "right"  + formatStr;    // (+x)
	faces[1] = std::string(cubeDir) + "left"   + formatStr;    // (-x)
	faces[2] = std::string(cubeDir) + "top"    + formatStr;    // (+y)
	faces[3] = std::string(cubeDir) + "bottom" + formatStr;    // (-y)
	faces[4] = std::string(cubeDir) + "front"  + formatStr;    // (-z)
	faces[5] = std::string(cubeDir) + "back"   + formatStr;    // (+z)

    Cubemap::format = format;
}

void Cubemap::BindTextures(){
    // Create and bind texture for cube map
    glGenTextures(1, &cubeMapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

    // Set texture type
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Set texture clamping to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// Potential help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Disable vertical flip since cubemaps start at top left corner
    stbi_set_flip_vertically_on_load(false);

    int fileType = GL_RGB;
    if (format == PNG) {
        fileType = GL_RGBA;
    }

    // Read and load each texture
    for (unsigned int i = 0; i < 6; i++) {
        int width, height, numChannels;
        
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &numChannels, 0);
        // Check if data exists
        if (data) {
            // Greate GL texture
            glTexImage2D(
                // Goes to each axis over iteration
                // +x, -x, +y, -y, +z, -z
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                0,
                GL_RGB,
                width,
                height,
                0,
                fileType,
                GL_UNSIGNED_BYTE,
                data 
            );
            // Unload data
            stbi_image_free(data);
        } else {
            // Print error message if data is unsuccessfully loaded
            std::cout << "Failed to load texture: " << faces[i] << std::endl;
			stbi_image_free(data);
        }
    }
}

void Cubemap::Draw(
    Shader & shader, Camera & camera, 
    glm::mat4 matrix, glm::vec3 translation,
    glm::quat rotation, glm::vec3 scale
){
    // Activate Shader
    shader.Activate();

    // Send camera position to shader and perform camera matrix calculations
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);
    camera.Matrix(shader, "camMatrix");

    // Create transformation matrices for mesh
    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);

    // Apply translation, rotation, and scale to transformation matrices
    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    // Send transformation matrices and model matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

    // Draw object 
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // Unbind and reset depth test after drawing
    glBindVertexArray(0);
}

void Cubemap::DrawSkybox(Shader & shader, Camera & camera, int width, int height){
    // Draw Skybox
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL); // Lequal is used since skybox has a z value of 1 (limit)
    shader.Activate();
    // Set view and projection matrices similar to camera set-up
    glm::mat4 skybox_view = glm::mat4(1.0f);
    glm::mat4 skybox_projection = glm::mat4(1.0f);
    // Scale view to mat3 then back to mat4
    // This sets rightmost column and bottom row to 0 to be unaffected by translation
    skybox_view = glm::mat4(glm::mat3(glm::lookAt(camera.position, camera.position + camera.orientation, camera.up)));
    skybox_projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    // Send view and projection to shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(skybox_view));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(skybox_projection));

    // Draw object 
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // Unbind and reset depth test after drawing
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); 
    glDepthMask(GL_TRUE);
}
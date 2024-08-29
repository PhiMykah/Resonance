#include "Line.h"

Line::Line(){

}

Line::Line(Vertices& vertices, Indices& indices){
    Line::initLines(vertices, indices);
}

void Line::initLines(Vertices& vertices, Indices& indices){
    Line::vertices = vertices;
    Line::indices = indices;

    // Vertex array object MUST be created before vertex buffer object
    // Vertex buffer is a different kind of buffer than the front and back buffer
    
    // Bind Vertex Array Object (VAO)
    vao.Bind();

    // Vertex Buffer Object (VBO)
    VBO vbo(vertices);
    // Index Buffer Object (EBO)
    EBO ebo(indices);

    // Link vbo layouts to corresponding vao
    // Position Coordinate layout (layout 0)
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
    // Color layout (layout 1)
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)(6 * sizeof(float)));
    
    // Unbind vao, vbo, and ebo to avoid further modifications
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

void Line::Draw(
    Shader& shader, Camera& camera,
    glm::mat4 matrix,
    glm::vec3 translation,
    glm::quat rotation,
    glm::vec3 scale
){

    // Activate shader and bind vao to shader
    shader.Activate();
    vao.Bind();

    // Send camera position to shader and perform camera matrix calculations
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);
    camera.Matrix(shader, "camMatrix");

    // Create transformation matrices for mesh

    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);

    // Global transformation matrices

    glm::mat4 gtrans = glm::mat4(1.0f);
    glm::mat4 grot = glm::mat4(1.0f);
    glm::mat4 gsca = glm::mat4(1.0f);

    // Apply translation, rotation, and scale to transformation matrices
    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    // Send transformation matrices and model matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));


    glDrawElements(primative, indices.size(), GL_UNSIGNED_INT, 0);
}
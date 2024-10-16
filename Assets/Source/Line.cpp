#include "Line.hpp"
#include "Constants.hpp"

Line::Line(){

}

Line::Line(LVertices& vertices, Indices& indices){
    Line::initLines(vertices, indices);
}

void Line::initLines(LVertices& vertices, Indices& indices){
    Line::vertices = vertices;
    Line::indices = indices;

    // Vertex array object MUST be created before vertex buffer object
    // Vertex buffer is a different kind of buffer than the front and back buffer
    
    // Bind Vertex Array Object (VAO)
    vao.Bind();

    // Vertex Buffer Object (VBO)
    VBO<LineVertex> vbo(vertices);
    // Index Buffer Object (EBO)
    EBO ebo(indices);

    // Link vbo layouts to corresponding vao
    // Position Coordinate layout (layout 0)
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(LineVertex), (void *)0);
    // Color layout (layout 1)
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(LineVertex), (void *)(3 * sizeof(float)));
    
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
    shader.setVec3("camPos", camera.position.x, camera.position.y, camera.position.z);
    camera.Matrix(shader, "camMatrix");

    // Create transformation matrices for mesh

    glm::mat4 trans = MAT_IDENTITY;
    glm::mat4 rot = MAT_IDENTITY;
    glm::mat4 sca = MAT_IDENTITY;

    // Apply translation, rotation, and scale to transformation matrices
    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    // Send transformation matrices and model matrix to shader
    shader.setMat4("translation", trans);
    shader.setMat4("rotation", rot);
    shader.setMat4("scale", sca);
    shader.setMat4("model", matrix);


    glDrawElements(primative, indices.size(), GL_UNSIGNED_INT, 0);
}
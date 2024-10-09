#include "FBO.hpp"

GLuint SelectionFBO::currSel = 0;

FBO::FBO()
{
}

void FBO::Init(int width, int height)
{
    // Create the FBO
    glGenFramebuffers(1, &ID);
    glBindFramebuffer(GL_FRAMEBUFFER, ID);

    // Create the texture object for the primitive information buffer
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, width, height, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);

    // Create the texture object for the depth buffer
    glGenTextures(1, &depthID);
    glBindTexture(GL_TEXTURE_2D, depthID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,depthID, 0);

    // Verify that the FBO is correct
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Frame Buffer error, status: 0x%x\n", status);
    }

    // Restore the default framebuffer
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::Bind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ID);
}

void FBO::Unbind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FBO::Delete()
{
    glDeleteTextures(1, &texID);
    glDeleteTextures(1, &depthID);
    glDeleteFramebuffers(1, &ID);
}

void FBO::Resize(int newWidth, int newHeight)
{
    // Delete the existing textures (both color and depth) and the framebuffer itself
    Delete();

    // Re-initialize with the new dimensions
    Init(newWidth, newHeight);
}

FBO::Pixel FBO::ReadPixel(GLuint x, GLuint y)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    FBO::Pixel Pixel;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &Pixel);

    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    return Pixel;
}

void SelectionFBO::InitVAO(void *meshPtr, VAO<PosVertex> &vao)
{
    
    // Bind Vertex Array Object (VAO)
    vao.Bind();

    NMRMesh * mesh = static_cast<NMRMesh *>(meshPtr);

    std::vector<GLuint> indices = mesh->indices;
    std::vector<PosVertex> vertices = mesh->posVertices;
    VBO<PosVertex> vbo(vertices);

    // Index Buffer Object (EBO)
    EBO ebo(indices);

    // Link vbo layouts to corresponding vao
    // Position Coordinate layout (layout 0)
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(PosVertex), (void *)0);

    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

void SelectionFBO::SelectMesh(Shader &selection_shader, Camera &camera, std::map<std::string, void *> nmrMeshes)
{
    Bind(); // Bind FBO

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear FBO

    selection_shader.Activate();

    for (auto [file, ptr] : nmrMeshes) 
    {
        // Avoid null pointer
        if (ptr == NULL) continue;

        VAO<PosVertex> vao;

        NMRMesh * mesh = static_cast<NMRMesh *>(ptr);

        SelectionFBO::InitVAO(mesh, vao);

        GLuint objectID = static_cast<GLuint>(mesh->ID); // Assign unique object ID starting from 1
        glUniform1ui(glGetUniformLocation(selection_shader.ID, "objID"), objectID);

        // Draw the mesh
        DrawSelection(selection_shader, camera, vao, mesh);

        // mesh->Draw(selection_shader, camera, mat, pos, rot, scale);
    }

    Unbind(); // Unbind FBO
}

void SelectionFBO::DrawSelection(Shader &shader, Camera &camera, VAO<PosVertex> &vao, NMRMesh * mesh)
{
    glm::mat4 mat   = mesh->drawMat;
    glm::vec3 pos   = mesh->pos;
    glm::quat rot   = mesh->rot;
    glm::vec3 scale = mesh->scale;

    shader.Activate();
    vao.Bind();

    camera.Matrix(shader, "camMatrix");

    // Create transformation matrices for mesh

    glm::mat4 trans = MAT_IDENTITY;
    glm::mat4 rotat = MAT_IDENTITY;
    glm::mat4 sca = MAT_IDENTITY;

    // Global transformation matrices

    glm::mat4 gtrans = MAT_IDENTITY;
    glm::mat4 grot = MAT_IDENTITY;
    glm::mat4 gsca = MAT_IDENTITY;

    // Apply translation, rotation, and scale to transformation matrices
    trans = glm::translate(trans, pos);
    rotat = glm::mat4_cast(rot);
    sca = glm::scale(sca, scale);

    // Send transformation matrices and model matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rotat));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(mat));

    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
}

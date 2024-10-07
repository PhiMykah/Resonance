#include "FBO.hpp"

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
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &Pixel);

    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    return Pixel;
}

void SelectionFBO::SelectMesh(Shader & selection_shader, Camera & camera, std::vector<NMRMesh> nmrMeshes)
{
    Bind(); // Bind FBO

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear FBO

    selection_shader.Activate();

    for (size_t i = 0; i < nmrMeshes.size(); i++)
    {
        GLuint objectID = static_cast<GLuint>(i + 1); // Assign unique object ID starting from 1
        glUniform1ui(glGetUniformLocation(selection_shader.ID, "objID"), objectID);

        glm::mat4 mat   = nmrMeshes[i].drawMat;
        glm::vec3 pos   = nmrMeshes[i].pos;
        glm::quat rot   = nmrMeshes[i].rot;
        glm::vec3 scale = nmrMeshes[i].scale;

        // Draw the mesh
        nmrMeshes[i].Draw(selection_shader, camera, mat, pos, rot, scale);
    }

    Unbind(); // Unbind FBO
}  

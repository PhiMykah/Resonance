#include "texture.h"

/*
Main texture constructor for creating OpenGL texture object

Parameters
----------
image : const char *
    Image file path represented as character array

texType : GLenum
    Dataype of texture, such as "GL_TEXTURE_2D"

slot : GLenum
    Texture unit or container to place texture in

format : GLenum
    Color Channel format of image (e.g. GL_RGB, GL_RGBA)

pixelType : GLenum
    Format in which the pixels for the texture are stored (e.g. GL_UNSIGNED_BYTE)

minLOD : GLenum
    Texture level of detail minimization algorithm (e.g. GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST)

magLOD : GLenum
    Texture level of detail magnification algorithm (e.g. GL_NEAREST, GL_LINEAR)

Returns
-------
Texture object
*/
Texture::Texture(const char * image, GLenum texType, GLuint slot, GLenum format, GLenum pixelType, GLenum minLOD, GLenum magLOD){
    // Assigns the type of the texture ot the texture object
    type = texType;
    int width, height, numColCh;
    stbi_set_flip_vertically_on_load(true);
    // Loads image using file path while populating the following parameters:
    // width, height, number of color channels
    unsigned char* bytes = stbi_load(image, &width, &height, &numColCh, 0);

    // ************************
    // * Load OpenGL Textures *
    // ************************
    // Generate 1 texture with given ID
    glGenTextures(1, &ID);

    // Activate texture unit, aka the texture container
    // Texture containers hold about 16 textures concurrently
    glActiveTexture(GL_TEXTURE0 + slot); // Use texture container based on slot
    unit = slot;
    glBindTexture(texType, ID); // Bind texture to texture container based on slot

    // ********************
    // * Texture Settings *
    // ********************

    // Choose image scale processing style, e.g. nearest neighbor or linear interpolation
    // Parameters are:
    //  - Type of texture
    //  - Texture attriubute we wish to modify
    //  - Updated setting for texture attribute
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, minLOD);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, magLOD);

    // Choose image repeat style, e.g. repeat or nearest repeat
    // Options are: GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, and GL_CLAMP_TO_BORDER
    // These image repeat settings are on a per axis basis
    //  - Each axis is labelled s, t, and r for x,y, and z axes respectively
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture border color
    // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

    // **********************
    // * Texture Generation *
    // **********************
    // Generate texture using the following parameters:
    //  - Type of texture
    //  - Texture level
    //  - Target Color channels for texture
    //  - Texture width
    //  - Texture height
    //  - Texture border (legacy compatibility)
    //  - Color channels the image has (RGB for image formats without alpha channels)
    //  - Pixel Datatype
    glTexImage2D(texType, 0, GL_RGBA, width, height, 0, format, pixelType, bytes);

    // Generate mipmap will generate multiple versions of the texture at varying sizes for distance
    glGenerateMipmap(texType);

    // Delete image data from memory after allocation
    stbi_image_free(bytes);

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(texType, 0);
}

/*
Allocate texture to texture unit and link coordinates

Parameters
----------
shader : Shader&
    Targe shader to activate when binding texture unit

uniform : const char *
    Name of uniform sampler2D variable

unit : GLuint
    ID of texture unit

Returns
-------
None
*/
void Texture::texUnit(Shader& shader, const char *uniform, GLuint unit)
{
    // Obtain sampler2D var from fragment shader for use in prgram
    GLuint texUni = glGetUniformLocation(shader.ID, uniform);
    shader.Activate();
    glUniform1i(texUni, unit); // Connect texUni to texture unit in main program after activation

}

/*
Bind texture to binding point

Parameters
----------
None

Returns
-------
None
*/
void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(type, ID);
}

/*
Unbind texture from binding point

Parameters
----------
None

Returns
-------
None
*/
void Texture::Unbind()
{
    glBindTexture(type, 0);
}

/*
Set OpenGL to remove texture

Parameters
----------
None

Returns
-------
None
*/
void Texture::Delete()
{
    glDeleteTextures(1, &ID);
}

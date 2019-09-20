#include "render.h"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

geometry makeGeometry(vertex* verts, size_t vertCount, unsigned * indices, size_t indxCount) {
	//create an instance of geometry
	geometry newGeo = {};
	newGeo.size = indxCount;
	//generate buffers
	glGenVertexArrays(1, &newGeo.vao);
	glGenBuffers(1, &newGeo.vbo);
	glGenBuffers(1, &newGeo.ibo);
	glBindVertexArray(newGeo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, newGeo.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newGeo.ibo);
	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(vertex), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxCount * sizeof(unsigned), indices, GL_STATIC_DRAW);
	//bing buffers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)16);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)32);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)48);
	//populate buffers
	//describe vertex data
	//unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//return the geometry
	return newGeo;
}

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char *pixels)
{
	GLenum oglFormat = GL_RGBA;

	switch (channels)
	{
	case 1:
		oglFormat = GL_RED;
		break;
	case 2:
		oglFormat = GL_RG;
		break;
	case 3:
		oglFormat = GL_RGB;
		break;
	case 4:
		oglFormat = GL_RGBA;
		break;
	default:
		assert(false);
		break;
	}

	texture tex = { 0, width, height, channels };
	glGenTextures(1, &tex.handle);
	glBindTexture(GL_TEXTURE_2D, tex.handle);

	glTexImage2D(GL_TEXTURE_2D, 0, oglFormat, width, height, 0, oglFormat, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}
void freeShader(shader &shad)
{
	glDeleteProgram(shad.program);
	shad = {};
}


void freeGeomerty(geometry &geo) {
	glDeleteBuffers(1, &geo.vbo);
	glDeleteBuffers(1, &geo.ibo);
	glDeleteVertexArrays(1, &geo.vao);
	geo = {};
}

shader makeShader(const char * vertSource, const char * fragSource)
{
	//make the shader object
	shader newShad = {};
	newShad.program = glCreateProgram();
	//create the shaders
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	//compile the shaders
	glShaderSource(vert, 1, &vertSource, 0);
	glShaderSource(frag, 1, &fragSource, 0);
	glCompileShader(frag);
	//attach the shaders
	glAttachShader(newShad.program, vert);
	glAttachShader(newShad.program, frag);
	//link
	glLinkProgram(newShad.program);
	//delete
	glDeleteShader(vert);
	glDeleteShader(frag);
	//return
	return newShad;
	return shader();
}

void setUniform(const shader & shad, GLuint location, const glm::mat4 & value)
{
	glProgramUniformMatrix4fv(shad.program, location, 1, GL_FALSE, glm::value_ptr(value));
}

void setUniform(const shader & shad, GLuint location, const glm::vec3 &value)
{
	glProgramUniform3fv(shad.program, location, 1, glm::value_ptr(value));
}

void setUniform(const shader &shad, GLuint location, const texture &value, int textureSlot)
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, value.handle);
	glProgramUniform1i(shad.program, location, textureSlot);
}






void draw(const shader & shad, const geometry & geo)
{
	glUseProgram(shad.program);
	glBindVertexArray(geo.vao);
	glDrawElements(GL_TRIANGLES, geo.size, GL_UNSIGNED_INT, 0);
}

texture loadTexture(const char * imagePath)
{
	int imageWidth, imageHeight, imageFormat;
	unsigned char *rawPixelData = nullptr;

	stbi_set_flip_vertically_on_load(true);
	rawPixelData = stbi_load(imagePath, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	//todo check rawPixelData null

	texture tex = makeTexture(imageWidth, imageHeight, imageFormat, rawPixelData);

	stbi_image_free(rawPixelData);

	return tex;
}
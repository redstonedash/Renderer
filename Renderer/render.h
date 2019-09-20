#pragma once
#include "glew/GL/glew.h"
#include "glm/glm.hpp"

struct vertex
{
	glm::vec4 pos;
	glm::vec4 color;
	glm::vec4 norm;
	glm::vec2 uv;
};

struct geometry
{
	GLuint vao, vbo, ibo; //buffers
	GLuint size;		  //index count
};



struct shader
{
	GLuint program;
};

struct texture
{
	GLuint handle;
	unsigned int width, height, channels;
};

void draw(const shader &shad, const geometry &geo);

texture loadTexture(const char * imagePath);

geometry makeGeometry(vertex * verts, size_t vertCount,
				      unsigned * indices, size_t indexCount);
void freeGeomerty(geometry & geo);

shader makeShader(const char * vertSource, const char * frag);

void freeShader(shader &shad);

void setUniform(const shader &shad, GLuint location, const glm::mat4 &value);
void setUniform(const shader & shad, GLuint location, const glm::vec3 & value);
void setUniform(const shader & shad, GLuint location, const texture & value, int textureSlot);
void GLAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message,
	const void *userParam);
#pragma once
#include "stdafx.h"

GLuint loadTexture(const std::string& path);
GLuint loadTexture(const std::string& path,int magFilter,int minFilter);

GLuint createVertexArray();

template<typename T>
GLuint createArrayBuffer(const std::vector<T>& data) {
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER,buffer);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
	return buffer;
}

glm::vec3 extractTranslation(const glm::mat4& model);



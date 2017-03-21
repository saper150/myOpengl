#pragma once
#include "stdafx.h"
#include "Wraper.h"
#include <soil\SOIL.h>

struct TextureVertex {
	glm::vec3 pos;
	glm::vec2 UV;
};

struct ProgramTexture {
	ProgramTexture(GLuint program, const std::vector<TextureVertex>& points, const std::string& texturePath) :program(program)
	{
		MVPlocation = glGetUniformLocation(program, "MVP");
		textureSamplerLocation = glGetUniformLocation(program, "textureSampler");

		size = points.size();

		vertexArray = createVertexArray();

		texture = loadTexture(texturePath);
		vertexBuffer = createArrayBuffer(points);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	void draw(const glm::mat4& mvp) {
		glUseProgram(program);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniformMatrix4fv(MVPlocation, 1, GL_FALSE, glm::value_ptr(mvp));
		glBindVertexArray(vertexArray);
		glDrawArrays(type, 0, size);
		glBindVertexArray(0);
	}

	GLsizei size = 0;
	GLenum type = GL_TRIANGLES;

	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint texture;

	GLuint textureSamplerLocation;
	GLuint MVPlocation;
	GLuint program;
};

typedef SingleTransformationWraper<ProgramTexture> Sprite;
Sprite createSprite(const std::string texturePath) {
	std::vector<TextureVertex> v = {
		{ { 0,1,0 }, {0,1} },
		{ { 1,0,0 },{1,0} },
		{ { 0,0,0 },{0,0} },
		{ { 0,1,0 },{0,1} },
		{ { 1,1,0 },{1,1} },
		{ { 1,0,0 },{1,0} }
	};

	const ProgramTexture p(Shaders::getInstance().getProgram(Shaders::vertex::UV, Shaders::fragment::TEXTURE),v,texturePath);
	return Sprite(p);
}

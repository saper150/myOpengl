#pragma once
#include "stdafx.h"
#include "Wraper.h"
#include <soil\SOIL.h>
#include "Colider.h"


struct TextureVertex {
	glm::vec3 pos;
	glm::vec2 UV;
};

struct TextureModelPrototype {
	TextureModelPrototype(GLuint program, const std::vector<TextureVertex>& points, const std::string& texturePath) 
		:program(program),colider(createColider<TextureVertex>(points))
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

	class Instance {
	public:
		Instance(GLuint vertexArray, GLuint texture, GLuint textureSamplerLocation,GLuint MVPlocation,GLuint program,GLint size)
			:size(size),vertexArray(vertexArray),texture(texture),textureSamplerLocation(textureSamplerLocation),MVPlocation(MVPlocation),program(program)
		{}
		void draw(const glm::mat4& mvp) {
			glUseProgram(program);
			glBindTexture(GL_TEXTURE_2D, texture);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glUniformMatrix4fv(MVPlocation, 1, GL_FALSE, glm::value_ptr(mvp));
			glBindVertexArray(vertexArray);
			glDrawArrays(GL_TRIANGLES, 0, size);
			glBindVertexArray(0);
		}


	private:
		GLsizei size = 0;
		GLuint vertexArray;
		GLuint texture;
		GLuint textureSamplerLocation;
		GLuint MVPlocation;
		GLuint program;
	};

	TextureModelPrototype::Instance createInstance() const { return Instance(vertexArray, texture, textureSamplerLocation, MVPlocation, program, size); }

	GLsizei size = 0;
	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint texture;
	GLuint textureSamplerLocation;
	GLuint MVPlocation;
	GLuint program;
	const aabbColider colider;

};

typedef SingleTransformationWraper<TextureModelPrototype::Instance> Sprite;
typedef SingleTransformationWraper<TextureModelPrototype::Instance> Model;
typedef Bilboard<TextureModelPrototype::Instance> Particle;

TextureModelPrototype createSpritePrototype(const std::string texturePath);




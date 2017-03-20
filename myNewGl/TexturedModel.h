#pragma once
#include "stdafx.h"
#include <soil\SOIL.h>

struct TextureVertex {
	glm::vec3 pos;
	glm::vec2 UV;
};

GLuint loadTexture(const std::string path) {
	int width, height;
	auto deleater = [](uint8_t* image) { SOIL_free_image_data(image); };
	std::unique_ptr<uint8_t, decltype(deleater)> image(SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGBA), deleater);

	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.get());
	glGenerateMipmap(GL_TEXTURE_2D);

	return textureId;

}

struct ProgramTexture {
	ProgramTexture(GLuint program) :program(program)
	{
		MVPlocation = glGetUniformLocation(program, "MVP");
		textureSamplerLocation = glGetUniformLocation(program, "textureSampler");
	}

	void draw(int size) {
	}
	void record(const std::vector<TextureVertex>& points, const std::string& texturePath) {
		size = points.size();
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		texture = loadTexture(texturePath);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(TextureVertex), points.data(), GL_STATIC_DRAW);

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

class TextureModel {
public:
	TextureModel(const std::vector<TextureVertex>& data,const std::string& texturePath)
		:program(Shaders::getInstance().getProgram<ProgramTexture>(1,1))
	{
		program.record(data, texturePath);
	}
	void draw(const Camera& camera) {
		glm::mat4 mvp = camera.projection()*camera.view()*transform*rotation;
		program.draw(mvp);
	}
	glm::mat4 transform;
	glm::mat4 rotation = glm::rotate(glm::mat4(),glm::radians(-90.f),glm::vec3(1,0,0));

private:
	ProgramTexture program;

};

TextureModel createSprite(const std::string texturePath) {
	std::vector<TextureVertex> v = {
		{{ 0,1,0 }, {0,1} },
		{ { 1,0,0 },{1,0} },
		{ { 0,0,0 },{0,0} },
		{ { 0,1,0 },{0,1} },
		{ { 1,1,0 },{1,1} },
		{ { 1,0,0 },{1,0} }
	};
	return TextureModel(v, texturePath);
}

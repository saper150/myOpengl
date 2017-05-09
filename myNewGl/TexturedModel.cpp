#include "stdafx.h"
#include "TexturedModel.h"


TextureModelPrototype createSpritePrototype(const std::string texturePath) {
	std::vector<TextureVertex> v = {
		{ { -1,1,0 },{ 0,1 } },
		{ { 1,-1,0 },{ 1,0 } },
		{ { -1,-1,0 },{ 0,0 } },
		{ { -1,1,0 },{ 0,1 } },
		{ { 1,1,0 },{ 1,1 } },
		{ { 1,-1,0 },{ 1,0 } }
	};
	return TextureModelPrototype(Shaders::getInstance().getProgram(Shaders::vertex::UV, Shaders::fragment::TEXTURE), v, texturePath);
}

TextureModelPrototype::TextureModelPrototype(GLuint program, const std::vector<TextureVertex>& points, const std::string & texturePath) :program(program), colider(createColider<TextureVertex>(points))
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

TextureModelPrototype::Bilboard TextureModelPrototype::createBilboard() const {
	return Bilboard(this);
}

TextureModelPrototype::Instance TextureModelPrototype::createInstance() const
{
	return Instance(this);
}
TextureModelPrototype::Instance::Instance(const TextureModelPrototype * parent) :parent(parent){}

void TextureModelPrototype::Instance::draw(const glm::mat4& mvp) {
	glUseProgram(parent->program);
	glBindTexture(GL_TEXTURE_2D, parent->texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, parent->texture);
	glUniformMatrix4fv(parent->MVPlocation, 1, GL_FALSE, glm::value_ptr(mvp));
	glBindVertexArray(parent->vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, parent->size);
	glBindVertexArray(0);
}

TextureModelPrototype::Bilboard::Bilboard(const TextureModelPrototype * parent) :Instance(parent){}

LigthModelPrototype::LigthModelPrototype(const std::vector<LigthVertex>& points, const std::string & texturePath) : colider(createColider<LigthVertex>(points)), programDescription(Shaders::getInstance().getProgram(PROGRAMS::LIGTH_PROGRAM))
{

	size = points.size();
	vertexArray = createVertexArray();

	texture = loadTexture(texturePath);
	vertexBuffer = createArrayBuffer(points);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LigthVertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LigthVertex), (GLvoid*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(LigthVertex), (GLvoid*)(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

}

LigthModelPrototype::Instance LigthModelPrototype::createInstance() const
{
	return Instance(this);
}

LigthModelPrototype::Instance::Instance(const LigthModelPrototype * parent):parent(parent){}

void LigthModelPrototype::Instance::draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
{
	glUseProgram(parent->programDescription->program);
	glBindTexture(GL_TEXTURE_2D, parent->texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, parent->texture);

	glUniformMatrix4fv(parent->programDescription->uniformLocations.at("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(parent->programDescription->uniformLocations.at("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(parent->programDescription->uniformLocations.at("model"), 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(parent->vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, parent->size);
	glBindVertexArray(0);
}
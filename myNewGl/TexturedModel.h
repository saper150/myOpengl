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
		Instance(const TextureModelPrototype* parent):parent(parent)
		{}
		void draw(const glm::mat4& mvp) {
			glUseProgram(parent->program);
			glBindTexture(GL_TEXTURE_2D, parent->texture);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, parent->texture);
			glUniformMatrix4fv(parent->MVPlocation, 1, GL_FALSE, glm::value_ptr(mvp));
			glBindVertexArray(parent->vertexArray);
			glDrawArrays(GL_TRIANGLES, 0, parent->size);
			glBindVertexArray(0);
		}
	private:
		const TextureModelPrototype* parent;
	};

	struct Bilboard :public TextureModelPrototype::Instance{
		Bilboard(const TextureModelPrototype* parent):Instance(parent)
		{	}
	};

	TextureModelPrototype::Bilboard createBilboard() const { return Bilboard(this); }
	TextureModelPrototype::Instance createInstance() const { return Instance(this); }

	GLsizei size = 0;
	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint texture;
	GLuint textureSamplerLocation;
	GLuint MVPlocation;
	GLuint program;
	aabbColider colider;

};


struct LigthVertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 UV;
};







struct LigthModelPrototype {
	LigthModelPrototype(const std::vector<LigthVertex>& points, const std::string& texturePath)
		:colider(createColider<LigthVertex>(points)),programDescription(Shaders::getInstance().getProgram(PROGRAMS::LIGTH_PROGRAM))
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

	class Instance {
	public:
		Instance(const LigthModelPrototype* parent) :parent(parent)
		{}
		void draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) {
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
	private:
		const LigthModelPrototype* parent;
	};

	LigthModelPrototype::Instance createInstance() const { return Instance(this); }

	GLsizei size = 0;
	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint texture;
	const ProgramDescription * programDescription;
	aabbColider colider;

};



TextureModelPrototype createSpritePrototype(const std::string texturePath);




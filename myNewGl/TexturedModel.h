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
	TextureModelPrototype(GLuint program, const std::vector<TextureVertex>& points, const std::string& texturePath);

	class Instance {
	public:
		Instance(const TextureModelPrototype* parent);
		void draw(const glm::mat4& mvp);
	private:
		const TextureModelPrototype* parent;
	};

	struct Bilboard :public TextureModelPrototype::Instance{
		Bilboard(const TextureModelPrototype* parent);
	};

	TextureModelPrototype::Bilboard createBilboard() const;
	TextureModelPrototype::Instance createInstance() const;

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
	LigthModelPrototype(const std::vector<LigthVertex>& points, const std::string& texturePath);
		

	class Instance {
	public:
		Instance(const LigthModelPrototype* parent);
		void draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);
	private:
		const LigthModelPrototype* parent;
	};

	LigthModelPrototype::Instance createInstance() const;

	GLsizei size = 0;
	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint texture;
	const ProgramDescription * programDescription;
	aabbColider colider;

};


TextureModelPrototype createSpritePrototype(const std::string texturePath);




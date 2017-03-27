#pragma once
#include "stdafx.h"
#include "TexturedModel.h"
#include "modelLoading.h"
class Stearing
{
public:
	Stearing(const glm::mat4* target);
	~Stearing();

	void update(float time);
	void draw(const Camera& camera);
private:
	glm::vec3 seek(const glm::vec3& target, const glm::vec3& pos, const glm::vec3& vel);
	glm::vec3 seperation(const int index);

	glm::vec3 automatonLocatio{0,0,0};
	glm::vec3 automatonVel{ 0,0,0 };

	TextureModelPrototype awesomeFaceProto = createSpritePrototype("kappa2.png");
	Particle automaton = awesomeFaceProto.createInstance();
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> vels;
	const glm::mat4 * target;

	const size_t count = 50;
	const float maxVel = 9.5f;
	const float maxAcc = 1.5f;
	const float seperationRadius = 1.f;

};


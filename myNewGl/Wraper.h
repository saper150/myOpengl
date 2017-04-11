#pragma once
#include "stdafx.h"


struct Position {
	glm::vec3 pos = { 0,0,0 };
};
struct Scale {
	float scale = 1.f;
};
struct Rotation
{
	glm::quat quat{};
};

glm::mat4 createModel(const Position& pos,const Rotation& rotation, const Scale& scale) {
	const auto translation = glm::translate(pos.pos);
	const auto roteted = translation * glm::mat4_cast(rotation.quat);
	return  glm::scale(roteted, { scale.scale,scale.scale,scale.scale });
}
glm::mat4 createModel(const Position& pos, const Rotation& rotation) {
	const auto translation = glm::translate(pos.pos);
	return translation * glm::mat4_cast(rotation.quat);
}
glm::mat4 createModel(const Position& pos, Scale& scale) {
	const auto translation = glm::translate(pos.pos);
	return  glm::scale(translation, { scale.scale,scale.scale,scale.scale });
}
glm::mat4 createModel(const Position& pos) {
	return glm::translate(pos.pos);
}


struct Transformation {
	glm::vec3 position;
	glm::quat rotation{};
	float scale = 1.0f;

	glm::mat4 createModel() {
		const auto translation = glm::translate(position);

		const auto roteted = translation * glm::mat4_cast(rotation);
		return  glm::scale(roteted, { scale,scale,scale });
	}
};




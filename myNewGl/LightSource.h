#pragma once
#include <glm\glm.hpp>
#include <entityx\entityx.h>

#include "Shaders.h"

using namespace entityx;

struct Light {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;
};


struct LightSystem :public System<LightSystem> {


	const std::vector<ProgramDescription*> programs;
	LightSystem(const std::vector<ProgramDescription*>& programs):programs(programs)
	{
			
	}

	void update(entityx::EntityManager &es, entityx::EventManager &events, TimeDelta dt) override {
		constexpr int maxLights = 5;

		int lightCount = 0;
		std::array<glm::vec3,maxLights> positions;
		std::array<glm::vec3, maxLights> ambient;
		std::array<glm::vec3, maxLights> diffuse;
		std::array<glm::vec3, maxLights> specular;
		std::array<GLfloat, maxLights> constant;
		std::array<GLfloat, maxLights> linear;
		std::array<GLfloat, maxLights> quadratic;

		es.each<Light,Position>(
			[&](Entity entity, Light &light,Position position) {
			positions[lightCount] = position.pos;
			ambient[lightCount] = light.ambient;
			diffuse[lightCount] = light.diffuse;
			specular[lightCount] = light.specular;
			constant[lightCount] = light.constant;
			linear[lightCount] = light.linear;
			quadratic[lightCount] = light.quadratic;
			lightCount++;

		});

		es.each<Light, Transformation>(
			[&](Entity entity, Light &light, Transformation transf) {
				positions[lightCount] = transf.position;
				ambient[lightCount] = light.ambient;
				diffuse[lightCount] = light.diffuse;
				specular[lightCount] = light.specular;
				constant[lightCount] = light.constant;
				linear[lightCount] = light.linear;
				quadratic[lightCount] = light.quadratic;
				lightCount++;

		});

		if (lightCount > maxLights) {
			throw std::runtime_error("max light exided");
		}
		for (auto description : programs) {
			const auto& location = description->uniformLocations;
			glUseProgram(description->program);
			glUniform3fv(location.at("light.position"), lightCount, reinterpret_cast<GLfloat*>(positions.data()));
			glUniform3fv(location.at("light.ambient"), lightCount, reinterpret_cast<GLfloat*>(ambient.data()));
			glUniform3fv(location.at("light.diffuse"), lightCount, reinterpret_cast<GLfloat*>(diffuse.data()));
			glUniform3fv(location.at("light.specular"), lightCount, reinterpret_cast<GLfloat*>(specular.data()));
			glUniform1fv(location.at("light.constant"), lightCount, constant.data());
			glUniform1fv(location.at("light.linear"), lightCount, linear.data());
			glUniform1fv(location.at("light.quadratic"), lightCount, quadratic.data());
			glUniform1i(location.at("lightCount"), lightCount);
		}


	}
};

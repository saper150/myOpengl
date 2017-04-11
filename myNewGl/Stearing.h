#pragma once
#include "stdafx.h"
#include "TexturedModel.h"
#include "modelLoading.h"

#include <glm\gtx\norm.hpp>
#include <future>
struct Stearing {
	struct Separation {

	};
	struct Flee {

	};
	struct SeekTarget {};
};


struct Velocity {
	glm::vec3 vel;
};

struct StearingSystem : public System<StearingSystem>
{
	float separationRadius = 3.f;
	float maxVel = 1000.f;
	float maxAcc = 500.f;
	float fleeRadius = 15.f;

	glm::vec3 separation(
		std::vector<Transformation*> trans,
		std::vector<Velocity*> velocities,
		size_t myIndex) 
	{
		glm::vec3 sum = { 0,0,0 };
		for (size_t i = 0; i < trans.size(); i++)
		{
			if (i == myIndex) continue;
			float distanceSquared = glm::distance2(trans[myIndex]->position, trans[i]->position);
			const float separationSquared = separationRadius*separationRadius;
			if (distanceSquared < separationSquared) {
				const auto desiredVel = glm::normalize(trans[myIndex]->position - trans[i]->position)*maxVel;
				sum += (desiredVel - velocities[myIndex]->vel) / distanceSquared;
			}
		}
		if (sum.x == 0 && sum.y == 0 && sum.z == 0) {
			return sum;
		}
		return glm::normalize(sum);
	}


	glm::vec3 separation(EntityManager &es, Transformation& current, Velocity &myVel) {
		glm::vec3 sum = { 0,0,0 };
		es.each<Transformation, Stearing::Separation>(
			[&](Entity entity, Transformation &other, Stearing::Separation separation)
		{
			float distanceSquared = glm::distance2(current.position, other.position);
			if (distanceSquared == 0) return;

			const float separationSquared = separationRadius*separationRadius;
			if (distanceSquared < separationSquared) {
				const auto desiredVel = glm::normalize(current.position - other.position)*maxVel;
				sum += (desiredVel - myVel.vel) / distanceSquared;
			}
		});
		if (sum.x == 0 && sum.y == 0 && sum.z == 0) {
			return sum;
		}
		return glm::normalize(sum);
	}

	glm::vec3 seek(EntityManager& es,const Transformation& current,const Velocity& currentVel) {
		glm::vec3 desiredVel;
		es.each<Transformation, Stearing::SeekTarget>(
			[&](Entity entity, Transformation &target, Stearing::SeekTarget)
		{
			desiredVel += target.position - current.position;
		});
		desiredVel = glm::normalize(desiredVel)*maxVel;
		return glm::normalize(desiredVel - currentVel.vel);
	}

	glm::vec3 flee(EntityManager& es, const Transformation& current, const Velocity& currentVel) {
		glm::vec3 desiredVel;
		es.each<Transformation, Stearing::Flee>(
			[&](Entity entity, Transformation &target, Stearing::Flee)
		{

			float distanceSquared = glm::distance2(target.position, current.position);
			if (distanceSquared < fleeRadius*fleeRadius) {
				desiredVel +=  (current.position-target.position);
			}
		});
		if (desiredVel.x == 0.f && desiredVel.x == 0.f && desiredVel.z == 0.f) {
			return{ 0,0,0 };
		}
		desiredVel = glm::normalize(desiredVel)*maxVel;
		return glm::normalize(desiredVel - currentVel.vel);
	}

	void update(entityx::EntityManager &es, entityx::EventManager &events, TimeDelta dt) override
	{
		std::vector<Transformation*> transformations;
		std::vector<Velocity*> velocities;
		std::vector<glm::vec3> seekForces;
		std::vector<glm::vec3> fleeForces;

		es.each<Transformation,Velocity, Stearing::Separation>(
			[&](Entity entity, Transformation &me,Velocity& vel, Stearing::Separation)
		{
			transformations.emplace_back(&me);
			velocities.emplace_back(&vel);
			seekForces.emplace_back(seek(es,me,vel));
			fleeForces.emplace_back(flee(es, me, vel));
		});

		std::vector<std::future<void>> handlers;
		for (size_t j = 0; j < transformations.size(); j++)
		{
			int i = j;
			handlers.emplace_back(std::async([&](int i) {
				const auto sepatationForce = separation(transformations, velocities, i);
				velocities[i]->vel += glm::normalize(0.5f*seekForces[i] + sepatationForce+fleeForces[i]*15.f)*maxAcc*static_cast<float>(dt);
				if (glm::length2(velocities[i]->vel) > maxVel*maxVel) {
					velocities[i]->vel = glm::normalize(velocities[i]->vel)*maxVel;
				}
			},j));

		}
		for (auto& h : handlers) {
			h.wait();
		}

		//transformations.clear();
	//	velocities.clear();
	//	seekForces.clear();
	};
};

struct VelocitySystem :public System<VelocitySystem> {
	void update(entityx::EntityManager &es, entityx::EventManager &events, TimeDelta dt) override
	{
		es.each<Transformation, Velocity>(
			[&](Entity entity, Transformation &pos, Velocity& vel)
		{
			pos.position += vel.vel*static_cast<float>(dt);

		});
	}

};


	//
	//class Stearing
	//{
	//public:
	//	Stearing(const glm::mat4* target);
	//	~Stearing();
	//
	//	void update(float time);
	//	void draw(const Camera& camera);
	//private:
	//	glm::vec3 seek(const glm::vec3& target, const glm::vec3& pos, const glm::vec3& vel);
	//	glm::vec3 seperation(const int index);
	//
	//	glm::vec3 automatonLocatio{0,0,0};
	//	glm::vec3 automatonVel{ 0,0,0 };
	//
	//	TextureModelPrototype awesomeFaceProto = createSpritePrototype("kappa2.png");
	//	Particle automaton = awesomeFaceProto.createInstance();
	//	std::vector<glm::vec3> positions;
	//	std::vector<glm::vec3> vels;
	//	const glm::mat4 * target;
	//
	//	const size_t count = 50;
	//	const float maxVel = 9.5f;
	//	const float maxAcc = 1.5f;
	//	const float seperationRadius = 1.f;
	//
	//};

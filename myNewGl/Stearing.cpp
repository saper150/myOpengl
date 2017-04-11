#include "stdafx.h"
#include "Stearing.h"

#include <glm\gtx\norm.hpp>
//
//Stearing::Stearing(const glm::mat4* target) :target(target)
//{
//	
//	positions.resize(count);
//	vels.resize(count);
//	for (size_t i = 0; i < count; i++)
//	{
//		positions[i] = randomVector();
//	}
//
//}
//
//
//Stearing::~Stearing()
//{
//}
//
//void Stearing::update(float time)
//{
//
//
//	 glm::vec3 targetLocation;
//	 targetLocation.x = (*target)[3][0];
//	 targetLocation.y = (*target)[3][1];
//	 targetLocation.z = (*target)[3][2];
//
//	 for (size_t i = 0; i < count; i++)
//	 {
//		 const glm::vec3 seekForce = seek(targetLocation, positions[i], vels[i]);
//		 std::cout << seekForce.x << "\t" << seekForce.y << "\t" << seekForce.z << "\n";
//
//		 const auto separationForce = seperation(i);
//
//		 const auto stearing = glm::normalize(seekForce + separationForce)*maxAcc*time;
//		 vels[i] += stearing;
//
//		 if (glm::length(vels[i]) > maxVel) {
//			 vels[i] = glm::normalize(vels[i])*maxVel;
//		 }
//		 
//		 positions[i] += vels[i]*time;
//	 }
//
//
//}
//void Stearing::draw(const Camera & camera)
//{
//
//	for (size_t i = 0; i < count; i++)
//	{
//		automaton.setTranslation(glm::translate(glm::mat4(), positions[i]));
//		automaton.draw(camera);
//	}
//}
//
//glm::vec3 Stearing::seek(const glm::vec3& target, const glm::vec3& pos, const glm::vec3& vel)
//{
//	auto desiredVel = glm::normalize(target - pos)*maxVel;
//	return glm::normalize(desiredVel - vel);
//}
//
//glm::vec3 Stearing::seperation(const int index)
//{
//	glm::vec3 sum = {0,0,0};
//	const glm::vec3& myPos = positions[index];
//	const glm::vec3& myVel = vels[index];
//
//	for (size_t i = 0; i < count; i++)
//	{
//		if (i == index) continue;
//		const glm::vec3& otherPos = positions[i];
//
//		float distanceSquared = glm::distance2(myPos, otherPos);
//		if (distanceSquared == 0) continue;
//
//		if (distanceSquared < seperationRadius*seperationRadius) {
//
//			const auto desiredVel = glm::normalize(myPos-otherPos)*maxVel;
//			sum += (desiredVel - myVel) / distanceSquared;
//		}
//	}
//	if (sum.x == 0 && sum.y == 0 && sum.z == 0) {
//		return sum;
//	}
//	return glm::normalize(sum);
//}
//

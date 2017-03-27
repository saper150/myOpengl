#pragma once
#include "stdafx.h"

class vec3;
class TransformationWraper;


class aabbColider
{
public:
	aabbColider(const glm::vec3& aabbMin, const glm::vec3& aabbMax)
		:aabbMin(aabbMin),aabbMax(aabbMax)
	{
	}
	~aabbColider();
	const glm::vec3 aabbMin;
	const glm::vec3 aabbMax;
	bool testRay (const glm::vec3& ray_origin, const glm::vec3& ray_direction,const glm::mat4& model, float& intersection_distance) const;
};

template <typename T>
aabbColider createColider(const std::vector<T>& points) {
	glm::vec3 min = points[0].pos;
	glm::vec3 max = points[0].pos;

	for (size_t i = 1; i < points.size(); i++)
	{

		for (size_t j = 0; j < 3; j++)
		{
			if (points[i].pos[j] < min[j]) {
				min[j] = points[i].pos[j];
			}
			if (points[i].pos[j] > max[j]) {
				max[j] = points[i].pos[j];
			}
		}

	}
	return aabbColider(min,max);


}

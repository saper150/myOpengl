#include "stdafx.h"
#include "myRandom.h"

std::default_random_engine engine{(unsigned int)std::chrono::high_resolution_clock::now().time_since_epoch().count()};

glm::vec3 randomVector() {
	static std::uniform_real_distribution<float> distribution(0.f, 1.f);
	return{ distribution(engine),distribution(engine) ,distribution(engine) };
}
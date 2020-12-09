#pragma once
#include <glm/vec3.hpp> 

class Ray
{
public:
	glm::vec3 orig;
	glm::vec3 dir;

	Ray(glm::vec3 origin, glm::vec3 dir);
};


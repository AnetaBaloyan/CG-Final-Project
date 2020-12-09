#include "Ray.h"
#include <glm/vec3.hpp>

Ray::Ray(glm::vec3 origin, glm::vec3 direction) {
	orig = origin;
	dir = direction;
}

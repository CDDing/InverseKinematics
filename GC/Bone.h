#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/GLU.h>
#include <GL/GL.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
enum Humanoid {
	LeftArm,
	RightArm,
	LeftLeg,
	RightLeg,
	Nothing,
};
class Bone {
public:
	Bone(glm::vec3 _center,glm::vec3 _direction,float _length);
	float length;

	glm::vec3 center;
	glm::vec3 direction;

	std::vector<Bone*> child;
	Bone* parent;
	glm::vec3 GetStart();
	glm::vec3 GetEnd();
};
Bone::Bone(glm::vec3 _center, glm::vec3 _direction, float _length) {
	center = _center;
	direction = _direction;
	length = _length;
}

glm::vec3 Bone::GetStart() {
	return center - 0.5f * length * glm::normalize(direction);
}
glm::vec3 Bone::GetEnd() {
	return center + 0.5f * length * glm::normalize(direction);
}
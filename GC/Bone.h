#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/GLU.h>
#include <GL/GL.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Bone {
public:
	Bone(glm::vec3 position,glm::vec3 direction);
	static float length;
	Bone* GetChild();
	Bone* GetParent();
	void SetPosition(glm::vec3 position);
	void SetDirect(glm::vec3 direction);
	void SetChild(Bone* bone);
	void SetParent(Bone* bone);
	glm::vec3 GetStart();
	glm::vec3 GetEnd();
	Bone* clone();
private:
	glm::vec3 start;
	glm::vec3 end;
	glm::vec3 direct;

	Bone* child;
	Bone* parent;
};
Bone::Bone(glm::vec3 position,glm::vec3 direction) {
	start = position;
	end = start + length*direction/glm::length(direction);
	direct = direction;
}
Bone* Bone::clone() {
	Bone* clone=new Bone(start,direct);
	return clone;
}
void Bone::SetPosition(glm::vec3 position) {
	start = position;
	end = start + length * direct / glm::length(direct);
}
void Bone::SetDirect(glm::vec3 direction) {
	direct = direction;
}
float Bone::length = 1.0f;

Bone* Bone::GetChild() {
	return child;
}
Bone* Bone::GetParent() {
	return parent;
}
void Bone::SetParent(Bone* bone) {
	parent = bone;
}
void Bone::SetChild(Bone* bone) {
	child = bone;
}
glm::vec3 Bone::GetStart() {
	return start;
}
glm::vec3 Bone::GetEnd() {
	return end;
}
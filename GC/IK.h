#pragma once
#include <queue>
#include <stack>
#include "Bone.h"
class IK {
public:
	float bias = 0.01f;
	glm::vec3 target;
	Bone* root;
	
	IK(Bone* _root);

	void solve();
};

IK::IK(Bone* _root) {
	root=_root;
}
void IK::solve() {
	std::stack<Bone*> st;
	Bone* tmp = root;
	float length = 0;
	while (true) {
		st.push(tmp);
		length += tmp->length;
		if (tmp->child.empty()) break;
		tmp = tmp->child[0];
	}
	if (glm::length(root->GetStart() - target) > length) {
		glm::vec3 direction = glm::normalize(target - root->GetStart());
		Bone* order = root;
		glm::vec3 position(root->GetStart());
		while (true) {
			order->direction = direction;
			order->center = position + 0.5f*direction * order->length;
			position =order->GetEnd();
			if (order->child.empty()) break;
			order = order->child[0];
		}
	}
	else {
		Bone* tail = st.top();
		Bone* order = st.top();
		glm::vec3 rememstart = root->GetStart();
		glm::vec3 temp = target;
		while (glm::length(root->GetStart() - rememstart) > bias ||
			glm::length(tail->GetEnd()  - target) > bias) {
			while (order) {
				float length = order->length;
				auto direction = temp - order->GetStart();
				order->direction = direction;
				order->center=(temp - 0.5f * length * glm::normalize(direction));
				if (order == root) break;
				temp = order->GetStart();
				order = order->parent;
			}
			if (glm::length(root->GetStart() - rememstart) < bias) {
				break;
			}

			order = root;
			temp = rememstart;
			while (true) {
				float length = order->length;
				auto direction = order->GetEnd() - temp;
				order->direction = direction;
				order->center = (temp + 0.5f * length * glm::normalize(direction));
				temp = order->GetEnd();
				if (order->child.empty()) break;
				order = order->child[0];
			}
		}
	}
}
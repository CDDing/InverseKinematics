#pragma once
#include <queue>
#include <stack>
#include "BoneTree.h"
class IK {
public:
	float bias = 0.01f;
	glm::vec3 target;
	BoneTree* tree;
	
	IK(BoneTree* BT);

	void solve();
};

IK::IK(BoneTree* BT) {
	tree = BT;
}
void IK::solve() {
	std::stack<Bone*> q;
	Bone* tmp = tree->GetRoot();
	float length = 0;
	while (tmp) {
		q.push(tmp);
		length += tmp->length;
		tmp = tmp->GetChild();
		
	}
	std::cout << length<<"\n";
	if (glm::length((tree->GetRoot()->GetStart() - target))> length) { //Bone들 길이의 총 합이 Target과의 거리보다 짧은 경우
		glm::vec3 direction = target - tree->GetRoot()->GetStart();
		Bone* order = tree->GetRoot();
		glm::vec3 position(0, 0, 0);
		while (order) {
			order->SetDirect(direction);
			order->SetPosition(position);
			position = order->GetEnd();
			order = order->GetChild();
		}
	}
	else {
		//Stage 1
		
		Bone* order = tree->GetTail();
		glm::vec3 rememstart = tree->GetRoot()->GetStart();
		glm::vec3 temp = target;

		

		while (glm::length(tree->GetRoot()->GetStart() - rememstart) > bias||
			glm::length(tree->GetTail()->GetEnd()-target)>bias) {
			while (order) {
				float length = order->length;
				auto direction = temp - order->GetStart();
				order->SetDirect(direction);
				order->SetPosition(temp - length * glm::normalize(direction));
				temp = order->GetStart();
				order = order->GetParent();
			}
			
			if (glm::length(tree->GetRoot()->GetStart() - rememstart) < bias) { //시작지점이 크게 바뀌지 않았으면 끝
				break;
			}

			//아닐경우 시작지점부터 끝지점까지 다시 순회

			order = tree->GetRoot();
			temp = rememstart;
			while (order) {
				float length = order->length;
				auto direction = order->GetEnd() - temp;
				order->SetDirect(direction);
				order->SetPosition(temp);
				temp = order->GetEnd();
				order = order->GetChild();
			}
		}


	}
}
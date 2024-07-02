#pragma once
#include "Bone.h"
class BoneTree {
public:
	BoneTree(Bone* bone);
	Bone* GetRoot();
	Bone* GetTail();
	void pushBone(Bone* bone);

private:
	Bone* Root;
	Bone* Tail;
	int BoneCount();
};
BoneTree::BoneTree(Bone* bone) {
	Root = bone;
	Tail = bone;
}

int BoneTree::BoneCount() {
	Bone* temp = Root;
	int cnt = 0;
	while (temp) {
		cnt++;
		temp = temp->GetChild();
	}
	return cnt;
}
void BoneTree::pushBone(Bone* bone) {
	bone->SetParent(Tail);
	bone->SetPosition(Tail->GetEnd());
	Tail->SetChild(bone);
	Tail = bone;
}
Bone* BoneTree::GetTail() {
	return Tail;
}
Bone* BoneTree::GetRoot() {
	return Root;
}
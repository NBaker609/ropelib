#ifndef ROPE_H
#define ROPE_H
#include <iostream>
#include <optional>
#include <vector>
#include <memory>
#include <string>

namespace rope
{
	inline size_t MAX_LEAF_LEN = 100;

	class RopeNode
	{
	public:
		std::unique_ptr<RopeNode> leftNode, rightNode;
		std::unique_ptr<std::string> data;
		RopeNode *parent;
		int leftLen;
		RopeNode();
	};

	struct LeafIndex
	{
		size_t localIndex;
		char characterAtIndex;
		RopeNode *localNode = nullptr;
	};
	void createRopeNode(std::string &str, std::unique_ptr<RopeNode> &root, RopeNode *parent, size_t leftIndex, size_t rightIndex);
	void printRopeNode(RopeNode *root);
	std::string ropeToString(RopeNode* root);
	std::unique_ptr<RopeNode> concatRopeNode(std::unique_ptr<RopeNode> &leftNode, std::unique_ptr<RopeNode> &rightNode);
	std::optional<LeafIndex> getLeafAtIndexInfo(size_t i, RopeNode *root);
	std::optional<LeafIndex> getNextLeaf(RopeNode* leafToStart);
	void adjustWeights(RopeNode *root, int32_t delta);
	void insertAtIndex(size_t i, RopeNode *root, std::string str);
	void deleteAtIndex(size_t i, RopeNode *root);
	void setMaxLeafLen(size_t n);
}	

#endif
#include "../include/rope.h"
#include "include/util.h"
void rope::insertAtIndex(size_t i, RopeNode *root, std::string str)
{
	const int64_t newLineDelta {std::count(str.begin(), str.end(), NEWLINE)};
	const std::optional<LeafIndex> nodeAtIndex {getLeafAtIndexInfo(i, root)};

	if (!nodeAtIndex.has_value())
	{
		return;
	}

	RopeNode *leafNode {nodeAtIndex->localNode};

	if (nodeAtIndex->localIndex >= MAX_LEAF_LEN || leafNode->data->length() >= MAX_LEAF_LEN || leafNode->data->length() + str.length() >= MAX_LEAF_LEN)
	{
		std::unique_ptr leftSideOfSplitNode {std::make_unique<RopeNode>()};
		std::unique_ptr rightSideOfSplitNode {std::make_unique<RopeNode>()};

		leftSideOfSplitNode->parent = leafNode;
		rightSideOfSplitNode->parent = leafNode;

		std::string leftSplitString {""};
		std::string rightSplitString {""};

		leftSplitString = leafNode->data->substr(0, nodeAtIndex->localIndex);
		leftSideOfSplitNode->leftLen = leftSplitString.length();
		rightSplitString = leafNode->data->substr(nodeAtIndex->localIndex, leafNode->data->length());
		leafNode->data->erase(leafNode->data->begin());
		leftSideOfSplitNode->data = std::make_unique<std::string>(leftSplitString);
		leafNode->data = nullptr;

		if (str.length() + rightSplitString.length() > MAX_LEAF_LEN)
		{
			std::string rightHandString {str + rightSplitString};
			createRope(rightHandString, rightSideOfSplitNode, nodeAtIndex->localNode, 0, rightHandString.length());
			leafNode->leftNode = std::move(leftSideOfSplitNode);
			leafNode->rightNode = std::move(rightSideOfSplitNode);

			leafNode->leftLen = leftSplitString.length();
			adjustWeights(leafNode, str.length(), newLineDelta);

			return;
		}

		rightSideOfSplitNode->data = std::make_unique<std::string>(str + rightSplitString);
		leafNode->leftNode = std::move(leftSideOfSplitNode);
		leafNode->rightNode = std::move(rightSideOfSplitNode);

		leafNode->leftLen = leftSplitString.length();
		adjustWeights(leafNode, str.length(), newLineDelta);

		return;
	}
	leafNode->data->insert(nodeAtIndex->localIndex, str);
	leafNode->leftLen += str.length();
	adjustWeights(leafNode, str.length(), newLineDelta);
}



void rope::deleteAtIndex(size_t i, RopeNode *root)
{
	std::optional<LeafIndex> leafAtIndex {getLeafAtIndexInfo(i, root)};
	if (!leafAtIndex.has_value())
		return;

	RopeNode *leafNode {leafAtIndex.value().localNode};
	int weightDelta {-1};
	int newLineDelta {0};
	if (leafAtIndex->characterAtIndex == NEWLINE)
		newLineDelta = -1;

	adjustWeights(leafNode, weightDelta, newLineDelta);
	leafNode->data->erase(leafAtIndex->localIndex, 1);
	if (leafNode->data->empty())
	{
		if (leafNode->parent == nullptr)
		{
			return;
		}
		else if (leafNode->parent->leftNode.get() == leafNode)
		{
			RopeNode *parent {leafNode->parent};
			RopeNode *rightChild {leafNode->parent->rightNode.get()};
			parent->move(rightChild);
			return;
		}
		else if (leafNode->parent->rightNode.get() == leafNode)
		{
			RopeNode *parent {leafNode->parent};
			RopeNode *leftChild {leafNode->parent->leftNode.get()};
			parent->move(leftChild);
			return;
		}
	}
}
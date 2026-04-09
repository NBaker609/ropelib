#include "util.h"
std::optional<rope::LeafIndex> searchDownLeftTree(rope::RopeNode *nodeToStart)
{
	rope::RopeNode *curNode {nodeToStart};
	while (curNode && curNode->data == nullptr)
	{
		if (curNode->leftNode)
			curNode = curNode->leftNode.get();
		else
			curNode = curNode->rightNode.get();
	}
	if (!curNode)
	{
		return std::nullopt;
	}

	rope::LeafIndex leafInfoToReturn;
	leafInfoToReturn.localNode = curNode;
	leafInfoToReturn.characterAtIndex = curNode->data->at(0);
	leafInfoToReturn.localIndex = 0;

	return leafInfoToReturn;
}


void adjustWeights(rope::RopeNode *leaf, int32_t delta, int32_t newLineDelta)
{
	if (leaf == nullptr)
	{
		return;
	}
	rope::RopeNode *parent {leaf->parent};
	if (parent == nullptr)
	{
		return;
	}

	if (parent->leftNode.get() == leaf)
	{
		parent->leftLen += delta;	
		parent->newLineCount += newLineDelta;
	}

	adjustWeights(parent, delta, newLineDelta);
}
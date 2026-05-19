#include "../include/rope.h"
#include "include/util.h"

// TODO: Fix this abomination
void rope::insertAtIndex(size_t i, RopeNode *root, std::string str, INSERT_SPECIFIER IS)
{
	const int64_t newLineDelta {std::count(str.begin(), str.end(), NEWLINE)};
	std::optional<LeafIndex> nodeAtIndex {}; 
	RopeNode* leafNode {};
	if(IS != INSERT_SPECIFIER::LOCAL_INSERT)
	{
	 	nodeAtIndex = getLeafAtIndexInfo(i, root);
		leafNode = nodeAtIndex->localNode;
	}
	else
	{
		nodeAtIndex = std::make_optional<LeafIndex>();
		nodeAtIndex.value().localIndex = i;
		leafNode = root;
	}
	if (!nodeAtIndex.has_value())
	{
		return;
	}

	if (nodeAtIndex->localIndex >= MAX_LEAF_LEN || leafNode->data->length() >= MAX_LEAF_LEN || leafNode->data->length() + str.length() >= MAX_LEAF_LEN)
	{
		i = nodeAtIndex->localIndex;

		std::unique_ptr leftSideOfSplitNode {std::make_unique<RopeNode>()};
		std::unique_ptr rightSideOfSplitNode {std::make_unique<RopeNode>()};

		leftSideOfSplitNode->parent = leafNode;
		rightSideOfSplitNode->parent = leafNode;

		std::string leftSplitString {""};
		std::string rightSplitString {""};

		const size_t centerOfLeaf { leafNode->data->length()/2 };

		leftSplitString = leafNode->data->substr(0, centerOfLeaf);
		leftSideOfSplitNode->leftLen = leftSplitString.length();

		const size_t lineCount { std::count(leftSplitString.begin(), leftSplitString.end(), NEWLINE) };
		leftSideOfSplitNode->newLineCount = lineCount;
		
		rightSplitString = leafNode->data->substr(centerOfLeaf);
		
		const size_t lineCoutRight { std::count(rightSplitString.begin(), rightSplitString.end(), NEWLINE) };
		rightSideOfSplitNode->leftLen = rightSplitString.length();
		rightSideOfSplitNode->newLineCount = lineCoutRight;

		rightSideOfSplitNode->data = std::make_unique<std::string>(rightSplitString);
		leftSideOfSplitNode->data = std::make_unique<std::string>(leftSplitString);
		leafNode->data = nullptr;

		if(i > leftSplitString.length())
		{
			i -= leftSplitString.length();
			if(str.length() + rightSplitString.length() > MAX_LEAF_LEN)
			{
				rightSideOfSplitNode->data->insert(i, str);
				createRope(*rightSideOfSplitNode->data, rightSideOfSplitNode, leafNode, 0, rightSplitString.length());
				leafNode->leftNode = std::move(leftSideOfSplitNode);
				leafNode->rightNode = std::move(rightSideOfSplitNode);
				
				leafNode->leftLen = leafNode->leftNode->leftLen;
				leafNode->newLineCount = leafNode->leftNode->newLineCount;

				adjustWeights(leafNode, str.length(), newLineDelta);
				return;
			}
			rightSideOfSplitNode->data->insert(i, str);
			leafNode->leftNode = std::move(leftSideOfSplitNode);
			leafNode->rightNode = std::move(rightSideOfSplitNode);
			leafNode->leftLen = leafNode->leftNode->leftLen;
			leafNode->newLineCount = leafNode->leftNode->newLineCount;
			adjustWeights(leafNode, str.length(), newLineDelta);
			return;
		}
		else if(str.length() + leftSplitString.length() > MAX_LEAF_LEN)
		{
			leftSideOfSplitNode->data->insert(i, str);
			createRope(*leftSideOfSplitNode->data, leftSideOfSplitNode, leafNode, 0, leftSplitString.length());
			leafNode->leftNode = std::move(leftSideOfSplitNode);
			leafNode->rightNode = std::move(rightSideOfSplitNode);
			
			leafNode->leftLen = leafNode->leftNode->leftLen;
			leafNode->newLineCount = leafNode->leftNode->newLineCount;


			adjustWeights(leafNode, str.length(), newLineDelta);
			return;
		}

		leftSideOfSplitNode->data->insert(i, str);
		leftSideOfSplitNode->leftLen += str.length();
		leftSideOfSplitNode->newLineCount += newLineDelta;
		leafNode->leftNode = std::move(leftSideOfSplitNode);
		leafNode->rightNode = std::move(rightSideOfSplitNode);
		leafNode->leftLen = leafNode->leftNode->leftLen;
		leafNode->newLineCount = leafNode->leftNode->newLineCount;
		adjustWeights(leafNode, str.length(), newLineDelta);

		return;
	}
	leafNode->data->insert(i, str);
	leafNode->leftLen += str.length();
	leafNode->newLineCount += newLineDelta;
	adjustWeights(leafNode->parent, str.length(), newLineDelta);
}

void rope::deleteAtIndex(size_t i, RopeNode *root, INSERT_SPECIFIER IS)
{
	std::optional<LeafIndex> leafAtIndex { getLeafAtIndexInfo(i, root) };
	if (!leafAtIndex.has_value())
		return;

	if(IS == INSERT_SPECIFIER::LOCAL_INSERT)
	{
		leafAtIndex->localNode = root;
		leafAtIndex->characterAtIndex = root->data->at(i);
	}

	RopeNode *leafNode {leafAtIndex.value().localNode};
	int weightDelta {-1};
	int newLineDelta {0};
	if (leafAtIndex->characterAtIndex == NEWLINE)
		newLineDelta = -1;

	adjustWeights(leafNode, weightDelta, newLineDelta);
	leafNode->data->erase(leafAtIndex->localIndex, 1);
	leafNode->leftLen += weightDelta;
	leafNode->newLineCount += newLineDelta;
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
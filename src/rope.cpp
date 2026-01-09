#include "../include/rope.h"

rope::RopeNode::RopeNode()
{
	leftNode = nullptr;
	rightNode = nullptr;
	parent = nullptr;
	data = nullptr;
	leftLen = 0;
}

void rope::createRopeNode(std::string &str, std::unique_ptr<RopeNode> &root, RopeNode *parent, size_t leftIndex, size_t rightIndex)
{
	root = std::make_unique<RopeNode>();
	root->parent = parent;
	if (rightIndex - leftIndex > MAX_LEAF_LEN)
	{
		int median = (rightIndex + leftIndex) / 2;
		root->leftLen = (rightIndex - leftIndex) / 2;
		createRopeNode(str, root->leftNode, root.get(), leftIndex, median);
		createRopeNode(str, root->rightNode, root.get(), median, rightIndex);
		return;
	}
	else
	{
		std::unique_ptr<std::string> data = std::make_unique<std::string>(str.substr(leftIndex, rightIndex - leftIndex));
		root->leftLen = data->length();
		root->data = std::move(data);
		return;
	}
}

void rope::printRopeNode(RopeNode *root)
{
	if (root == nullptr)
		return;

	if (root->leftNode == nullptr && root->rightNode == nullptr)
	{
		std::cout << *root->data;
		return;
	}
	printRopeNode(root->leftNode.get());
	printRopeNode(root->rightNode.get());
}

void rope::adjustWeights(RopeNode *leaf, int32_t delta)
{
	if (leaf == nullptr)
	{
		return;
	}
	RopeNode *parent = leaf->parent;
	if (parent == nullptr)
	{
		return;
	}

	if (leaf->parent->leftNode.get() == leaf)
		parent->leftLen += delta;
	adjustWeights(leaf->parent, delta);
}

void rope::insertAtIndex(size_t i, RopeNode *root, std::string str)
{
	std::optional<LeafIndex> nodeAtIndex = getLeafAtIndexInfo(i, root);

	if (!nodeAtIndex.has_value())
	{
		std::cout << "insert tried at invalid index\n";
		return;
	}

	RopeNode *leafNode = nodeAtIndex->localNode;

	if (nodeAtIndex->localIndex >= MAX_LEAF_LEN || leafNode->data->length() >= MAX_LEAF_LEN || leafNode->data->length() + str.length() >= MAX_LEAF_LEN)
	{
		std::unique_ptr leftSideOfSplitNode = std::make_unique<RopeNode>();
		std::unique_ptr rightSideOfSplitNode = std::make_unique<RopeNode>();

		leftSideOfSplitNode->parent = leafNode;
		rightSideOfSplitNode->parent = leafNode;

		std::string leftSplitString = leafNode->data->substr(0, nodeAtIndex->localIndex);
		leftSideOfSplitNode->leftLen = leftSplitString.length();
		std::string rightSplitString = leafNode->data->substr(nodeAtIndex->localIndex, leafNode->data->length());

		leftSideOfSplitNode->data = std::make_unique<std::string>(leftSplitString);
		leafNode->data = nullptr;

		if (str.length() + rightSplitString.length() > MAX_LEAF_LEN)
		{
			std::string rightHandString = str + rightSplitString;
			createRopeNode(rightHandString, rightSideOfSplitNode, nodeAtIndex->localNode, 0, rightHandString.length());
			leafNode->leftNode = std::move(leftSideOfSplitNode);
			leafNode->rightNode = std::move(rightSideOfSplitNode);

			leafNode->leftLen = leftSplitString.length();
			adjustWeights(leafNode->parent, str.length());

			return;
		}

		rightSideOfSplitNode->data = std::make_unique<std::string>(str + rightSplitString);
		leafNode->leftNode = std::move(leftSideOfSplitNode);
		leafNode->rightNode = std::move(rightSideOfSplitNode);

		leafNode->leftLen = leftSplitString.length();
		adjustWeights(leafNode->parent, str.length());

		return;
	}
	leafNode->data->insert(nodeAtIndex->localIndex, str);
	leafNode->leftLen += str.length();
	adjustWeights(leafNode->parent, str.length());
}
void rope::deleteAtIndex(size_t i, RopeNode* root)
{
	std::optional<LeafIndex> leafAtIndex = getLeafAtIndexInfo(i, root);
	if(!leafAtIndex.has_value())
		return;

	RopeNode* leafNode = leafAtIndex.value().localNode;
	leafNode->data->erase(leafAtIndex->localIndex, 1);
	adjustWeights(leafNode, -1);
}
void rope::setMaxLeafLen(size_t n)
{
	MAX_LEAF_LEN = n;
}
std::unique_ptr<rope::RopeNode> rope::concatRopeNode(std::unique_ptr<RopeNode> &leftNode, std::unique_ptr<RopeNode> &rightNode)
{
	std::unique_ptr<RopeNode> concattedRopeNode = std::make_unique<RopeNode>();
	concattedRopeNode->leftNode = std::move(leftNode);
	concattedRopeNode->rightNode = std::move(rightNode);
	concattedRopeNode->leftLen = leftNode->leftLen;
	return concattedRopeNode;
}

std::optional<rope::LeafIndex> rope::getLeafAtIndexInfo(size_t i, RopeNode *root)
{
	if (root == nullptr)
	{
		return std::nullopt;
	}
	if (root->leftNode == nullptr && root->rightNode == nullptr)
	{
		if (i >= root->data->length())
			return std::nullopt;
		LeafIndex leafInfo;
		leafInfo.characterAtIndex = root->data->at(i);
		leafInfo.localIndex = i;
		leafInfo.localNode = root;
		return leafInfo;
	}
	if (root->leftLen > i && root->leftNode.get() != nullptr)
	{
		return getLeafAtIndexInfo(i, root->leftNode.get());
	}
	else if (root->leftLen <= i && root->rightNode.get() != nullptr)
	{
		size_t newIndex = i - root->leftLen;
		return getLeafAtIndexInfo(newIndex, root->rightNode.get());
	}
	return std::nullopt;
}

std::optional<rope::LeafIndex> rope::getNextLeaf(RopeNode* leafToStart)
{
	RopeNode* parent = leafToStart->parent;
	if(parent == nullptr) return std::nullopt;
	if(parent->rightNode.get() == leafToStart)
	{
		return getNextLeaf(parent);
	}
	if(parent->leftNode.get() == leafToStart)
	{
		RopeNode* parentsRightChild = parent->rightNode.get();
		
		if(parentsRightChild == nullptr) 
			return rope::getNextLeaf(parent);
		
		RopeNode* curNode = parentsRightChild;
		while(curNode->data == nullptr)
		{
			curNode = curNode->leftNode.get();
		}
		LeafIndex leafInfoToReturn;

		leafInfoToReturn.localNode = curNode;
		leafInfoToReturn.characterAtIndex = curNode->data->at(0);
		leafInfoToReturn.localIndex = 0;
		return leafInfoToReturn;
	}
	return std::nullopt;
}
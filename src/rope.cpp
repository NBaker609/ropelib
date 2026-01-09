#include "rope.h"

const int MAX_LEAF_LEN = 100;

rope::Rope::Rope()
{
	leftNode = nullptr;
	rightNode = nullptr;
	parent = nullptr;
	data = nullptr;
	leftLen = 0;
}

void rope::createRope(std::string &str, std::unique_ptr<Rope> &root, Rope *parent, size_t leftIndex, size_t rightIndex)
{
	root = std::make_unique<Rope>();
	root->parent = parent;
	if (rightIndex - leftIndex > MAX_LEAF_LEN)
	{
		int median = (rightIndex + leftIndex) / 2;
		root->leftLen = (rightIndex - leftIndex) / 2;
		createRope(str, root->leftNode, root.get(), leftIndex, median);
		createRope(str, root->rightNode, root.get(), median, rightIndex);
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

void rope::printRope(Rope *root)
{
	if (root == nullptr)
		return;

	if (root->leftNode == nullptr && root->rightNode == nullptr)
	{
		std::cout << *root->data;
		return;
	}
	printRope(root->leftNode.get());
	printRope(root->rightNode.get());
}

std::unique_ptr<rope::Rope> rope::concatRope(std::unique_ptr<Rope> &leftNode, std::unique_ptr<Rope> &rightNode)
{
	std::unique_ptr<Rope> concattedRope = std::make_unique<Rope>();
	concattedRope->leftNode = std::move(leftNode);
	concattedRope->rightNode = std::move(rightNode);
	concattedRope->leftLen = leftNode->leftLen;
	return concattedRope;
}

std::optional<rope::LeafIndex> rope::getLeafAtIndexInfo(size_t i, Rope *root)
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
		return std::optional<LeafIndex>(leafInfo);
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
void rope::adjustWeights(Rope *root, int32_t delta)
{
	if (root  == nullptr)
	{
		return;
	}
	Rope* parent = root->parent;
	if(parent == nullptr)
	{
		return;
	}

	if (root->parent->leftNode.get() == root)
		parent->leftLen += delta;
	adjustWeights(root->parent, delta);
}
void rope::insertAtIndex(size_t i, Rope *root, std::string str)
{
	std::optional<LeafIndex> nodeAtIndex = getLeafAtIndexInfo(i, root);

	if (!nodeAtIndex.has_value())
	{
		std::cout << "insert tried at invalid index\n";
		return;
	}

	Rope *leafNode = nodeAtIndex->localNode;

	if (nodeAtIndex->localIndex >= MAX_LEAF_LEN || leafNode->data->length() >= MAX_LEAF_LEN || leafNode->data->length() + str.length() >= MAX_LEAF_LEN)
	{
		std::unique_ptr leftSideOfSplitNode = std::make_unique<Rope>();
		std::unique_ptr rightSideOfSplitNode = std::make_unique<Rope>();

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
			createRope(rightHandString, rightSideOfSplitNode, nodeAtIndex->localNode, 0, rightHandString.length());
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
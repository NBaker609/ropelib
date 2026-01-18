#include "../include/rope.h"

void rope::createRope(std::string &str, std::unique_ptr<RopeNode> &root, RopeNode *parent, size_t leftIndex, size_t rightIndex)
{
	root = std::make_unique<RopeNode>();
	root->parent = parent;
	if (rightIndex - leftIndex > MAX_LEAF_LEN)
	{
		const size_t median = (rightIndex + leftIndex) / 2;
		root->leftLen = (rightIndex - leftIndex) / 2;

		std::string substr = str.substr(leftIndex, root->leftLen);
		size_t n = std::count(substr.begin(), substr.end(), NEWLINE);
		root->newLineCount = n;

		createRope(str, root->leftNode, root.get(), leftIndex, median);
		createRope(str, root->rightNode, root.get(), median, rightIndex);
		return;
	}
	else
	{
		std::unique_ptr<std::string> data = std::make_unique<std::string>(str.substr(leftIndex, rightIndex - leftIndex));
		root->leftLen = data->length();
		size_t n = std::count(data->begin(), data->end(), NEWLINE);
		root->newLineCount = n;
		root->data = std::move(data);
		return;
	}
}

std::string rope::getRopeAsString(RopeNode *root)
{
	std::string ropeString("");
	RopeNode* curNode = getLeafAtIndexInfo(0, root).value().localNode;
	while (true)
	{
		ropeString.append(*curNode->data);
		std::optional<LeafIndex> nextLeaf = getNextLeaf(curNode);
		if(nextLeaf.has_value())
			curNode = nextLeaf.value().localNode;
		else
			break;
	}
	return ropeString;
	
}

void rope::adjustWeights(const RopeNode *leaf, int32_t delta, int32_t newLineDelta)
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

	if (parent->leftNode.get() == leaf)
	{
		parent->leftLen += delta;
		parent->newLineCount += newLineDelta;
	}

	adjustWeights(parent, delta, newLineDelta);
}

void rope::insertAtIndex(size_t i, RopeNode *root, std::string str)
{
	size_t newLineDelta = std::count(str.begin(), str.end(), NEWLINE);
	std::optional<LeafIndex> nodeAtIndex = getLeafAtIndexInfo(i, root);

	if (!nodeAtIndex.has_value())
	{
		return;
	}

	RopeNode *leafNode = nodeAtIndex->localNode;

	if (nodeAtIndex->localIndex >= MAX_LEAF_LEN || leafNode->data->length() >= MAX_LEAF_LEN || leafNode->data->length() + str.length() >= MAX_LEAF_LEN)
	{
		std::unique_ptr leftSideOfSplitNode = std::make_unique<RopeNode>();
		std::unique_ptr rightSideOfSplitNode = std::make_unique<RopeNode>();

		leftSideOfSplitNode->parent = leafNode;
		rightSideOfSplitNode->parent = leafNode;

		std::string leftSplitString("");
		std::string rightSplitString("");
		
		leafNode->data->insert(0, " "); // <- so if for whatever reason the leafNode we are splitting on contains an empty string, we append a space at the beginning so substr has no chance of breaking
		leftSplitString = leafNode->data->substr(0, nodeAtIndex->localIndex);
		leftSideOfSplitNode->leftLen = leftSplitString.length();
		rightSplitString = leafNode->data->substr(nodeAtIndex->localIndex, leafNode->data->length());
		leafNode->data->erase(leafNode->data->begin());
		leftSideOfSplitNode->data = std::make_unique<std::string>(leftSplitString);
		leafNode->data = nullptr;

		if (str.length() + rightSplitString.length() > MAX_LEAF_LEN)
		{
			std::string rightHandString = str + rightSplitString;
			createRope(rightHandString, rightSideOfSplitNode, nodeAtIndex->localNode, 0, rightHandString.length());
			leafNode->leftNode = std::move(leftSideOfSplitNode);
			leafNode->rightNode = std::move(rightSideOfSplitNode);

			leafNode->leftLen = leftSplitString.length();
			adjustWeights(leafNode->parent, str.length(), newLineDelta);

			return;
		}

		rightSideOfSplitNode->data = std::make_unique<std::string>(str + rightSplitString);
		leafNode->leftNode = std::move(leftSideOfSplitNode);
		leafNode->rightNode = std::move(rightSideOfSplitNode);

		leafNode->leftLen = leftSplitString.length();
		adjustWeights(leafNode->parent, str.length(), newLineDelta);

		return;
	}
	leafNode->data->insert(nodeAtIndex->localIndex, str);
	leafNode->leftLen += str.length();
	adjustWeights(leafNode->parent, str.length(), newLineDelta);
}
void rope::deleteAtIndex(size_t i, RopeNode *root)
{
	std::optional<LeafIndex> leafAtIndex = getLeafAtIndexInfo(i, root);
	if (!leafAtIndex.has_value())
		return;

	const RopeNode *leafNode = leafAtIndex.value().localNode;
	int weightDelta = -1;
	int newLineDelta = 0;
	if (leafAtIndex->characterAtIndex == NEWLINE)
		newLineDelta = -1;
	
	adjustWeights(leafNode, weightDelta, newLineDelta);
	leafNode->data->erase(leafAtIndex->localIndex, 1);
	if(leafNode->data->empty())
	{
		if(leafNode->parent == nullptr)
		{
			return;
		}
		else if(leafNode->parent->leftNode.get() == leafNode)
		{
			RopeNode* parent {leafNode->parent};
			RopeNode* rightChild = leafNode->parent->rightNode.get();
			parent->move(rightChild);
			return;
		}
		else if(leafNode->parent->rightNode.get() == leafNode)
		{
			RopeNode* parent = leafNode->parent;
			RopeNode* leftChild = leafNode->parent->leftNode.get();
			parent->move(leftChild);
			return;
		}
	}
}
void rope::setMaxLeafLen(size_t n)
{
	rope::MAX_LEAF_LEN = n;
}

std::optional<std::string> rope::getLine(size_t i, RopeNode *root)
{

	std::optional<LeafIndex> lineStartingPoint = getNewLineAtIndex(i, root);

	if (!lineStartingPoint.has_value())
	{
		return std::nullopt;
	}

	int index = 0;
	if (lineStartingPoint->characterAtIndex == NEWLINE)
		index = lineStartingPoint->localIndex + 1;
	else
		index = lineStartingPoint->localIndex;

	const RopeNode *curNode = lineStartingPoint->localNode;
	std::string lineString("");

	while (true)
	{
		if (curNode->data->empty())
		{
			std::optional<LeafIndex> nextNode = getNextLeaf(curNode);
			if (!nextNode.has_value())
				return lineString;
			index = 0;
			curNode = nextNode.value().localNode;
		}
		if (index >= curNode->data->length())
		{
			std::optional<LeafIndex> nextNode = getNextLeaf(curNode);
			if (!nextNode.has_value())
				return lineString;
			index = 0;
			curNode = nextNode.value().localNode;
		}
		if (curNode->data->at(index) == NEWLINE)
		{
			return lineString;
		}

		lineString.push_back(curNode->data->at(index));
		index++;
	}
	return lineString;
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

std::optional<rope::LeafIndex> rope::getNewLineAtIndex(size_t i, RopeNode *root)
{
	if (root == nullptr)
	{
		return std::nullopt;
	}

	if (root->leftNode == nullptr && root->rightNode == nullptr)
	{
		if (i > root->newLineCount)
			return std::nullopt;

		int numOfNewLinesFound = 0;
		for (size_t index = 0; index < root->data->length(); ++index)
		{
			if (root->data->at(index) == NEWLINE)
			{
				numOfNewLinesFound++;
			}
			if (numOfNewLinesFound == i)
			{
				numOfNewLinesFound = index;
				break;
			}
		}

		LeafIndex newLineLocationInfo;
		newLineLocationInfo.localIndex = numOfNewLinesFound;
		newLineLocationInfo.localNode = root;
		newLineLocationInfo.characterAtIndex = root->data->at(numOfNewLinesFound);
		return newLineLocationInfo;
	}
	if (root->newLineCount >= i && root->leftNode.get() != nullptr)
	{
		return getNewLineAtIndex(i, root->leftNode.get());
	}
	else if (root->newLineCount < i && root->rightNode.get() != nullptr)
	{
		size_t newIndex = i - root->newLineCount;
		return getNewLineAtIndex(newIndex, root->rightNode.get());
	}

	return std::nullopt;
}

std::optional<rope::LeafIndex> searchDownLeftTree(rope::RopeNode *nodeToStart)
{
	rope::RopeNode *curNode = nodeToStart;
	while (curNode && curNode->data == nullptr)
	{
		if(curNode->leftNode)
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
std::optional<rope::LeafIndex> rope::getNextLeaf(const RopeNode *leafToStart)
{
	const RopeNode *parent = leafToStart->parent;
	if (parent == nullptr)
		return std::nullopt;
	if (parent->rightNode.get() == leafToStart)
	{
		return getNextLeaf(parent);
	}
	if (parent->leftNode.get() == leafToStart)
	{
		RopeNode *parentsRightChild = parent->rightNode.get();

		if (parentsRightChild == nullptr)
			return rope::getNextLeaf(parent);

		std::optional<LeafIndex> leafWalkedDownLeft = searchDownLeftTree(parentsRightChild);
		while (true)
		{
			if (!leafWalkedDownLeft.has_value())
			{
				if(parentsRightChild->rightNode == nullptr)
					return std::nullopt;
				parentsRightChild = parentsRightChild->rightNode.get();
				leafWalkedDownLeft = searchDownLeftTree(parentsRightChild);
			}
			else
			{
				return leafWalkedDownLeft;
			}
		}

		return leafWalkedDownLeft;
	}
	return std::nullopt;
}
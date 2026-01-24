#include "../include/rope.h"
#include "include/util.h"
// All get functions
std::string rope::getRopeAsString(RopeNode *root)
{
	std::string ropeString {""};
	const std::optional<LeafIndex> opt {getLeafAtIndexInfo(0, root)};
	if (!opt.has_value())
		return ropeString;
	RopeNode *curNode {opt.value().localNode};
	while (true)
	{
		ropeString.append(*curNode->data);
		const std::optional<LeafIndex> nextLeaf {getNextLeaf(curNode)};
		if (nextLeaf.has_value())
			curNode = nextLeaf.value().localNode;
		else
			break;
	}
	return ropeString;
}



std::optional<std::string> rope::getLine(size_t i, RopeNode *root)
{

	std::optional<LeafIndex> lineStartingPoint {getNewLineAtIndex(i, root)};

	if (!lineStartingPoint.has_value())
	{
		return std::nullopt;
	}

	int index {0};
	if (lineStartingPoint->characterAtIndex == NEWLINE)
		index = lineStartingPoint->localIndex + 1;
	else
		index = lineStartingPoint->localIndex;

	RopeNode *curNode {lineStartingPoint->localNode};
	std::string lineString {""};

	while (true)
	{
		if (curNode->data->empty())
		{
			const std::optional<LeafIndex> nextNode {getNextLeaf(curNode)};
			if (!nextNode.has_value())
				return lineString;
			index = 0;
			curNode = nextNode.value().localNode;
		}
		if (index >= curNode->data->length())
		{
			const std::optional<LeafIndex> nextNode {getNextLeaf(curNode)};
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


std::optional<rope::LeafIndex> rope::getLeafAtIndexInfo(size_t i, RopeNode *root)
{
	if (root == nullptr)
	{
		return std::nullopt;
	}
	if (root->leftNode == nullptr && root->rightNode == nullptr)
	{
		if (i >= root->data->length() && i != 0)
			return std::nullopt;
		LeafIndex leafInfo;
		if (i != 0)
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
		const size_t newIndex {i - root->leftLen};
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

		size_t numOfNewLinesFound {0};
		for (size_t index {0}; index < root->data->length(); ++index)
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
		size_t newIndex {i - root->newLineCount};
		return getNewLineAtIndex(newIndex, root->rightNode.get());
	}

	return std::nullopt;
}


std::optional<rope::LeafIndex> rope::getNextLeaf(RopeNode *leafToStart)
{
	RopeNode *parent {leafToStart->parent};
	if (parent == nullptr)
		return std::nullopt;
	if (parent->rightNode.get() == leafToStart)
	{
		return getNextLeaf(parent);
	}
	if (parent->leftNode.get() == leafToStart)
	{
		RopeNode *parentsRightChild {parent->rightNode.get()};

		if (parentsRightChild == nullptr)
			return rope::getNextLeaf(parent);

		std::optional<LeafIndex> leafWalkedDownLeft {searchDownLeftTree(parentsRightChild)};
		while (true)
		{
			if (!leafWalkedDownLeft.has_value())
			{
				if (parentsRightChild->rightNode == nullptr)
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
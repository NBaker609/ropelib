#include "../include/rope.h"
// Core functions of the rope data structure. 
void rope::createRope(std::string &str, std::unique_ptr<RopeNode> &root, RopeNode *parent, size_t leftIndex, size_t rightIndex)
{
	root = std::make_unique<RopeNode>();
	root->parent = parent;
	if (rightIndex - leftIndex > MAX_LEAF_LEN)
	{
		const size_t median {(rightIndex + leftIndex) / 2};
		root->leftLen = (rightIndex - leftIndex) / 2;

		const std::string substr {str.substr(leftIndex, root->leftLen)};
		const int64_t n {std::count(substr.begin(), substr.end(), NEWLINE)};
		root->newLineCount = n;

		createRope(str, root->leftNode, root.get(), leftIndex, median);
		createRope(str, root->rightNode, root.get(), median, rightIndex);
		return;
	}
	else
	{
		size_t lengthOfData {rightIndex - leftIndex};
		std::unique_ptr<std::string> data {std::make_unique<std::string>(str.substr(leftIndex, rightIndex - leftIndex))};
		root->leftLen = data->length();
		int64_t n {std::count(data->begin(), data->end(), NEWLINE)};
		root->newLineCount = n;
		root->data = std::move(data);
		return;
	}
}



std::unique_ptr<rope::RopeNode> rope::concatRopeNode(std::unique_ptr<RopeNode> &leftNode, std::unique_ptr<RopeNode> &rightNode)
{
	std::unique_ptr<RopeNode> concattedRopeNode {std::make_unique<RopeNode>()};
	concattedRopeNode->leftNode = std::move(leftNode);
	concattedRopeNode->rightNode = std::move(rightNode);
	concattedRopeNode->leftLen = leftNode->leftLen;
	return concattedRopeNode;
}


void rope::setMaxLeafLen(size_t n)
{
	rope::MAX_LEAF_LEN = n;
}




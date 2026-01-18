#include "../include/rope.h"

rope::RopeNode::RopeNode()
{
    leftNode = nullptr;
	rightNode = nullptr;
	parent = nullptr;
	data = nullptr;
	leftLen = 0;
}

void rope::RopeNode::move(RopeNode* node)
{
    leftNode = std::move(node->leftNode);
    rightNode = std::move(node->rightNode);
    data = std::move(node->data);
    leftLen = node->leftLen;
    newLineCount = node->newLineCount;
}
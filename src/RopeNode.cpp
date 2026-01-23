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
    std::unique_ptr<RopeNode>* childPtr {nullptr};
    node->parent = this->parent;
    if(node == leftNode.get())
        childPtr = &leftNode;
    if(node == rightNode.get())
        childPtr = &rightNode;
    if(node->parent == nullptr)
    {
        data = std::move(node->data);
        if(data)
            leftLen = data->length();
        else
        {
            data = std::make_unique<std::string>("");
            leftLen = 0;
        }
        
        newLineCount = node->newLineCount;
        leftNode = std::move(node->leftNode);
        rightNode = std::move(node->rightNode);
        return;
    }
    if(parent->leftNode.get() == this)
    {
        parent->leftNode = std::move(*childPtr);
        return;
    }
    if(parent->rightNode.get() == this)
    {
        parent->rightNode = std::move(*childPtr);
        return;
    }
}
#ifndef ROPE_H
#define ROPE_H
#include <iostream>
#include <optional>
#include <vector>
#include <memory>
#include <string>

namespace rope
{
	class Rope
	{
	public:
		std::unique_ptr<Rope> leftNode, rightNode;
		std::unique_ptr<std::string> data;
		Rope *parent;
		int leftLen;
		Rope();
	};

	struct LeafIndex
	{
		size_t localIndex;
		char characterAtIndex;
		Rope *localNode = nullptr;
	};
	void createRope(std::string &str, std::unique_ptr<Rope> &root, Rope *parent, size_t leftIndex, size_t rightIndex);
	void printRope(Rope *root);
	std::unique_ptr<Rope> concatRope(std::unique_ptr<Rope> &leftNode, std::unique_ptr<Rope> &rightNode);
	std::optional<LeafIndex> getLeafAtIndexInfo(size_t i, Rope *root);
	void adjustWeights(Rope *root, int32_t delta);
	void insertAtIndex(size_t i, Rope *root, std::string str);
}

#endif
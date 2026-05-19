#ifndef ROPE_H
#define ROPE_H
#include <optional>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
namespace rope
{
	enum class INSERT_SPECIFIER
	{
		LOCAL_INSERT,
		GLOBAL_INSERT
	};

	inline const char  NEWLINE = '\n';
	 
	inline size_t MAX_LEAF_LEN {100};

	class RopeNode
	{
	public:
		std::unique_ptr<RopeNode> leftNode, rightNode;
		std::unique_ptr<std::string> data;
		RopeNode *parent;
		size_t leftLen, newLineCount;

		RopeNode();
		RopeNode& operator=(RopeNode &rightHand) = delete;
		RopeNode(const RopeNode& copy) = delete;
	
		void move(RopeNode* node);
	};

	struct LeafIndex
	{
		size_t localIndex;
		char characterAtIndex;
		RopeNode *localNode;
	};

	void createRope(std::string &str, std::unique_ptr<RopeNode> &root, RopeNode *parent, size_t leftIndex, size_t rightIndex);
	void insertAtIndex(size_t i, RopeNode *root, std::string str, INSERT_SPECIFIER IS = INSERT_SPECIFIER::GLOBAL_INSERT);
	void deleteAtIndex(size_t i, RopeNode *root, INSERT_SPECIFIER IS);
	void setMaxLeafLen(size_t n);
	
	std::string getRopeAsString(RopeNode *root);
	std::string getRestOfLine(size_t index, RopeNode* root);
	
	std::unique_ptr<RopeNode> concatRopeNode(std::unique_ptr<RopeNode> &leftNode, std::unique_ptr<RopeNode> &rightNode);
	
	std::optional<std::string> getLine(size_t i, RopeNode* root);
	std::optional<LeafIndex> getLeafAtIndexInfo(size_t i, RopeNode *root);
	std::optional<LeafIndex> getNextLeaf(RopeNode* leafToStart);
	std::optional<LeafIndex> getPrevLeaf(RopeNode* leafToStart);
	std::optional<LeafIndex> getNewLineAtIndex(size_t i, RopeNode* root);

}

#endif
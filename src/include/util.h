// Functions that are used by the API that do not need to be part of the API
#include "../../include/rope.h"

std::optional<rope::LeafIndex> searchDownLeftTree(rope::RopeNode *nodeToStart);
void adjustWeights(rope::RopeNode *leaf, int32_t delta, int32_t newLineDelta);
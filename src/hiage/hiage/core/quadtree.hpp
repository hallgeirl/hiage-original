#pragma once

#include "boundingbox.hpp"

#include <memory>
#include <cstdint>
#include <vector>

namespace hiage
{
    /*
        Inspired by: https://stackoverflow.com/questions/41946007/efficient-and-well-explained-implementation-of-a-quadtree-for-2d-collision-det
        Alternative: Go for hierarchical grid?
    */

    // This is the actual tree nodes. Bounding box is computed on the fly.
    struct QuadTreeNode
    {
        int32_t firstChildIndex;
        int32_t count;
    };

    // QuadTreeNodeData is a more complete representation of a node, which is returned from findLeaves and used in stack traversal.
    struct QuadTreeNodeData
    {
        int32_t depth = 0;
        BoundingBox<int32_t> boundingBox;
        int32_t index;
    };

    // The elements stored in the quad tree. Modelled as a linked list.
    struct QuadTreeElement
    {
        int32_t next;
        uint64_t entityId;
        BoundingBox<int32_t> boundingBox;
    };
    
    // The main quad tree class.
    class QuadTree
    {
    private:
        BoundingBox<int32_t> _boundingBox;
        int32_t _capacity;
        std::vector<QuadTreeNode> _nodes;
        int32_t _minWidth = 32, 
                _minHeight = 32;

        std::vector<QuadTreeElement> _elements; // TODO - what do we need to store here though? entity IDs?

    public:
        QuadTree() {}
        QuadTree(const BoundingBox<int32_t>& boundingBox, int capacity);

        std::vector<QuadTreeNodeData> findLeaves(const BoundingBox<int32_t>& boundingBox);
        
        bool insert(uint64_t entityId, const BoundingBox<int32_t>& boundingBox);
    };
}
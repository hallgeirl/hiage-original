#pragma once

#include <memory>
#include <cstdint>
#include <vector>

namespace hiage
{
    /*
        Inspired by: https://stackoverflow.com/questions/41946007/efficient-and-well-explained-implementation-of-a-quadtree-for-2d-collision-det
        Alternative: Go for hierarchical grid?
    */
    struct QuadTreeNode
    {
        int32_t firstChildIndex;
        int32_t count;
    };

    struct QuadTreeNodeData
    {
        int32_t depth = 0;
        int32_t left, bottom, right, top;
        int32_t index;
    };

    struct QuadTreeElement
    {
        int32_t next;
        uint64_t entityId;
        int32_t left, bottom, right, top;
    };
    
    class QuadTree
    {
    private:
        int32_t _left, _right, _top, _bottom;
        int32_t _capacity;
        std::vector<QuadTreeNode> _nodes;
        int32_t _minWidth = 32, 
                _minHeight = 32;

        std::vector<QuadTreeElement> _elements; // TODO - what do we need to store here though? entity IDs?

    public:
        QuadTree(int left, int bottom, int right, int top, int capacity) : _left(left), _right(right), _top(top), _bottom(bottom), _capacity(capacity)
        {
            _nodes.push_back(QuadTreeNode {
                .firstChildIndex = -1,
                .count = 0
            });
        }

        std::vector<QuadTreeNodeData> findLeaves(int32_t boundsLeft, int32_t boundsBottom, int32_t boundsRight, int32_t boundsTop);
        
        bool insert(uint64_t entityId, int32_t boundsLeft, int32_t boundsBottom, int32_t boundsRight, int32_t boundsTop);
    };
}
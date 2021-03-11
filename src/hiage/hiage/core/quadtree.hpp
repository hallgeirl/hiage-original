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

        std::vector<QuadTreeNodeData> findLeaves(int32_t boundsLeft, int32_t boundsBottom, int32_t boundsRight, int32_t boundsTop)
        {
            std::vector<QuadTreeNodeData> leaves;
            std::vector<QuadTreeNodeData> stack;

            stack.push_back(QuadTreeNodeData {
                .depth = 0,
                .left = _left, 
                .bottom = _bottom,
                .right = _right, 
                .top = _top,
                .index = 0
            });

            // We need to traverse the tree to find an appropriate node.
            while (stack.size() > 0)
            {
                auto nodeData = stack.back();
                stack.pop_back();

                auto& node = _nodes[nodeData.index];

                // Ignore objects that do not belong in this quad tree
                if (boundsRight < nodeData.left || boundsLeft > nodeData.right || boundsTop < nodeData.bottom || boundsBottom > nodeData.top)
                    continue; // Object is out of bounds - skip it

                int width = _right - _left;
                int height = _top - _bottom;

                // Is this a leaf node (count >= 0)? If so, try to insert.
                if (node.count >= 0)
                {
                    leaves.push_back(nodeData);
                }
                else
                {
                    // Node is not a leaf node => we must traverse!
                    auto firstChild = node.firstChildIndex;
                    /*
                        +0: Bottom left
                        +1: Bottom right
                        +2: Top left
                        +3: Top right
                    */
                    // Bottom left
                    stack.push_back(QuadTreeNodeData {
                        .depth = nodeData.depth+1,
                        .left = nodeData.left, 
                        .bottom = nodeData.bottom,
                        .right = nodeData.left + width / 2, 
                        .top = nodeData.bottom + height / 2,
                        .index = firstChild
                    });

                    // Bottom right
                    stack.push_back(QuadTreeNodeData {
                        .depth = nodeData.depth+1,
                        .left = nodeData.left + width / 2, 
                        .bottom = nodeData.bottom,
                        .right = nodeData.right, 
                        .top = nodeData.bottom + height / 2,
                        .index = firstChild + 1
                    });

                    // Top left
                    stack.push_back(QuadTreeNodeData {
                        .depth = nodeData.depth+1,
                        .left = nodeData.left, 
                        .bottom = nodeData.bottom + height / 2,
                        .right = nodeData.left + width / 2, 
                        .top = nodeData.top,
                        .index = firstChild + 2
                    });

                    // Top right
                    stack.push_back(QuadTreeNodeData {
                        .depth = nodeData.depth+1,
                        .left = nodeData.left + width / 2, 
                        .bottom = nodeData.bottom + height / 2,
                        .right = nodeData.right, 
                        .top = nodeData.top,
                        .index = firstChild + 2
                    });
                }
            }

            return leaves;
        }

        bool insert(uint64_t entityId, int32_t boundsLeft, int32_t boundsBottom, int32_t boundsRight, int32_t boundsTop);
    };
}
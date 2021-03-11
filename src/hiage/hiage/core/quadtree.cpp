#include "quadtree.hpp"

using namespace hiage;

std::vector<QuadTreeNodeData> QuadTree::findLeaves(int32_t boundsLeft, int32_t boundsBottom, int32_t boundsRight, int32_t boundsTop)
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

bool QuadTree::insert(uint64_t entityId, int32_t boundsLeft, int32_t boundsBottom, int32_t boundsRight, int32_t boundsTop)
{
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

        // Node is leaf, and is full, and node can be divided without dropping below minimum dimensions? If so, subdivide.
        if (node.count > _capacity && (width / 2 > _minWidth) && (height / 2 > _minHeight))
        {
            // Add new child leaf nodes
            _nodes.push_back(QuadTreeNode {
                .firstChildIndex = -1,
                .count = 0
            });
            _nodes.push_back(QuadTreeNode {
                .firstChildIndex = -1,
                .count = 0
            });
            _nodes.push_back(QuadTreeNode {
                .firstChildIndex = -1,
                .count = 0
            });
            _nodes.push_back(QuadTreeNode {
                .firstChildIndex = -1,
                .count = 0
            });

            // Redistribute elements (Remove all elements from this node, add them to the subsequent child nodes)
            if (node.firstChildIndex >= 0)
            {
                // Traverse linked list of elements
                uint32_t next = node.firstChildIndex;
                do
                {
                    auto* elem = &_elements[next];

                    
                    // Bottom left
/*                    stack.push_back(QuadTreeNodeData {
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
                    });*/


                    


                    next = elem->next;// todo: redistribute!
                } while (next != -1);
            }

            //This node is no longer a leaf.
            node.count = -1;

            // Point to the first child
            node.firstChildIndex = (int32_t)_nodes.size() - 4; 
        }

        // Is this a leaf node (count >= 0)? If so, try to insert.
        if (node.count >= 0)
        {
            // Node is not full, or node cannot be divided? Add object to this node
            _elements.push_back(QuadTreeElement {
                .next = node.firstChildIndex,
                .entityId = entityId,
                .left = boundsLeft,
                .bottom = boundsBottom,
                .right = boundsRight,
                .top = boundsTop
            });
            node.firstChildIndex = (int32_t)(_elements.size() - 1);
            node.count++;
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

    return true;
}
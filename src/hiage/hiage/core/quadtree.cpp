#include "quadtree.hpp"

using namespace hiage;

QuadTree::QuadTree(const BoundingBox<int32_t>& boundingBox, int capacity) : _boundingBox(boundingBox), _capacity(capacity)
{
    _nodes.push_back(QuadTreeNode {
        .firstChildIndex = -1,
        .count = 0
    });
}

// Creates a new bounding box with half the width and height of the original, offset by the specified amount
inline BoundingBox<int32_t> createSubdivision(const BoundingBox<int32_t>& boundingBox, int offsetX, int offsetY)
{
    auto width = boundingBox.width() >> 1;
    auto height = boundingBox.height() >> 1;
    auto offsetXAbsolute = offsetX * width;
    auto offsetYAbsolute = offsetY * height;
    return BoundingBox(boundingBox.left + offsetXAbsolute, 
                       boundingBox.right - width + offsetXAbsolute, 
                       boundingBox.top - width / 2 + offsetY, boundingBox.bottom + offsetY);
}

std::vector<QuadTreeNodeData> QuadTree::findLeaves(const BoundingBox<int32_t>& boundingBox)
{
    std::vector<QuadTreeNodeData> leaves;
    std::vector<QuadTreeNodeData> stack;

    stack.push_back(QuadTreeNodeData {
        .depth = 0,
        .boundingBox = boundingBox,
        .index = 0
    });

    // We need to traverse the tree to find an appropriate node.
    while (stack.size() > 0)
    {
        auto nodeData = stack.back();
        stack.pop_back();

        auto& node = _nodes[nodeData.index];

        // Ignore objects that do not belong in this quad tree
        if (!boundingBox.intersects(nodeData.boundingBox))
            continue; // Object is out of bounds - skip it

        int width = _boundingBox.width();
        int height = _boundingBox.height();

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
                .boundingBox = BoundingBox<int32_t>(nodeData.boundingBox.left, 
                                                    nodeData.boundingBox.left + width / 2, 
                                                    nodeData.boundingBox.bottom + height /2, 
                                                    nodeData.boundingBox.bottom),
                .index = firstChild
            });

            // Bottom right
            stack.push_back(QuadTreeNodeData {
                .depth = nodeData.depth+1,
                .boundingBox = BoundingBox<int32_t>(nodeData.boundingBox.left + width / 2,    //left
                                                    nodeData.boundingBox.right,               //right
                                                    nodeData.boundingBox.bottom + height / 2, //top
                                                    nodeData.boundingBox.bottom),             //bottom
                .index = firstChild + 1
            });

            // Top left
            stack.push_back(QuadTreeNodeData {
                .depth = nodeData.depth+1,
                .boundingBox = BoundingBox<int32_t>(nodeData.boundingBox.left,                //left
                                                    nodeData.boundingBox.left + width / 2,    //right
                                                    nodeData.boundingBox.bottom + height / 2, //top
                                                    nodeData.boundingBox.bottom),             //bottom
                .index = firstChild + 2
            });

            // Top right
            stack.push_back(QuadTreeNodeData {
                .depth = nodeData.depth+1,
                .boundingBox = BoundingBox<int32_t>(nodeData.boundingBox.left + width / 2      //left
                                                    nodeData.boundingBox.right,                //right
                                                    nodeData.boundingBox.top,                  //top
                                                    nodeData.boundingBox.bottom + height / 2), //bottom
                .index = firstChild + 2
            });
        }
    }

    return leaves;
}

bool QuadTree::insert(uint64_t entityId, const BoundingBox& boundingBox)
{
    std::vector<QuadTreeNodeData> stack;

    stack.push_back(QuadTreeNodeData {
        .depth = 0,
        .boundingBox = boundingBox,
        .index = 0
    });

    // We need to traverse the tree to find an appropriate node.
    while (stack.size() > 0)
    {
        auto nodeData = stack.back();
        stack.pop_back();

        auto& node = _nodes[nodeData.index];

        // Ignore objects that do not belong in this quad tree
        if (!boundingBox.intersects(nodeData.boundingBox))
            continue; // Object is out of bounds - skip it

        int width = _boundingBox.width();
        int height = _boundingBox.height();

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
                .boundingBox = createSubdivision(nodeData.boundingBox, 0, 0),
                .index = firstChild
            });

            // Bottom right
            stack.push_back(QuadTreeNodeData {
                .depth = nodeData.depth+1,
                .boundingBox = createSubdivision(nodeData.boundingBox, width / 2, 0),
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
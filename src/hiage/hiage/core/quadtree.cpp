#include "quadtree.hpp"

#include <stdexcept>
#include <sstream>

using namespace hiage;

void QuadTree::init(const BoundingBox<int32_t>& boundingBox, int capacity, DebugRenderer* debugRenderer)
{
    _boundingBox = boundingBox;
    _debugRenderer = debugRenderer;
    _capacity = capacity;

    _elements.clear();
    _nodes.clear();
    _nodes.push_back(QuadTreeNode {
        .firstChildIndex = -1,
        .count = 0
    });

    _minWidth = 16;
    _minHeight = 16;
}

void QuadTree::renderDebugInfo()
{
    std::vector<QuadTreeNodeData> leaves;
    if (_debugRenderer->getDebugFlags().quadTreeDebugFlags.drawChildCount || _debugRenderer->getDebugFlags().quadTreeDebugFlags.renderLeaves)
        leaves = findLeaves(_boundingBox);

    if (_debugRenderer->getDebugFlags().quadTreeDebugFlags.drawChildCount)
    {
        for (size_t i = 0; i < leaves.size(); i++)
        {
            auto&l = leaves[i];
            auto& node = _nodes[l.index];
            std::stringstream ss;
            ss << node.count;
            _debugRenderer->renderText(ss.str(), (l.boundingBox.left + l.boundingBox.right) / 2, (l.boundingBox.bottom + l.boundingBox.top) / 2);
        }
    }

    if (_debugRenderer->getDebugFlags().quadTreeDebugFlags.renderLeaves)
    {
        for (size_t i = 0; i < leaves.size(); i++)
        {
            auto&l = leaves[i];
            std::vector<Vec2d> verts;
            verts.push_back({ (double)l.boundingBox.left, (double)l.boundingBox.bottom });
            verts.push_back({ (double)l.boundingBox.left, (double)l.boundingBox.top });
            verts.push_back({ (double)l.boundingBox.right, (double)l.boundingBox.top });
            verts.push_back({ (double)l.boundingBox.right, (double)l.boundingBox.bottom });
            _debugRenderer->renderLines(verts);
        }
    }
}

QuadTreeNodeData createChildNode(const QuadTreeNodeData& parent, int32_t index, int32_t firstChild, int32_t nextWidth, int32_t nextHeight)
{
    if (index == 0) // Bottom left
        return QuadTreeNodeData {
            .depth = parent.depth+1,
            .boundingBox = BoundingBox<int32_t>(parent.boundingBox.left, 
                                                parent.boundingBox.bottom,
                                                parent.boundingBox.left + nextWidth, 
                                                parent.boundingBox.bottom + nextHeight),
            .index = firstChild
        };
    else if (index == 1) // Bottom right
        return QuadTreeNodeData {
            .depth = parent.depth+1,
            .boundingBox = BoundingBox<int32_t>(parent.boundingBox.left + nextWidth, 
                                                parent.boundingBox.bottom,
                                                parent.boundingBox.right, 
                                                parent.boundingBox.bottom + nextHeight),
            .index = firstChild + 1
        };

    else if (index == 2) // Top left
        return QuadTreeNodeData {
            .depth = parent.depth+1,
            .boundingBox = BoundingBox<int32_t>(parent.boundingBox.left,
                                                parent.boundingBox.bottom + nextHeight,
                                                parent.boundingBox.left + nextWidth,
                                                parent.boundingBox.top),
            .index = firstChild + 2
        };
    else if (index == 3) // Top right
        return QuadTreeNodeData {
            .depth = parent.depth+1,
            .boundingBox = BoundingBox<int32_t>(parent.boundingBox.left + nextWidth,
                                                parent.boundingBox.bottom + nextHeight,
                                                parent.boundingBox.right,
                                                parent.boundingBox.top),
            .index = firstChild + 3
        };
    throw std::runtime_error("quadTree.cpp: createChildNode: invalid index");
}


// Convenience function to add children of a node to the traversal stack
void insertChildrenToStack(std::vector<QuadTreeNodeData>& stack, const BoundingBox<int32_t>& boundingBox, const QuadTreeNodeData& nodeData, int32_t firstChild)
{
    auto nextWidth = nodeData.boundingBox.width() >> 1;
    auto nextHeight = nodeData.boundingBox.height() >> 1;

    // Bottom row
    if (boundingBox.bottom <= nodeData.boundingBox.bottom + nextHeight)
    {
        // Left column
        if (boundingBox.left <= nodeData.boundingBox.left + nextWidth)
        {
            stack.push_back(createChildNode(nodeData, 0, firstChild, nextWidth, nextHeight));
        }

        // Right column
        if (boundingBox.right > nodeData.boundingBox.left + nextWidth)
        {
            stack.push_back(createChildNode(nodeData, 1, firstChild, nextWidth, nextHeight));
        }
    }
    // Top row
    if (boundingBox.top > nodeData.boundingBox.bottom + nextHeight)
    {
        // Left column
        if (boundingBox.left <= nodeData.boundingBox.left + nextWidth)
        {
            stack.push_back(createChildNode(nodeData, 2, firstChild, nextWidth, nextHeight));
        }

        // Right column
        if (boundingBox.right > nodeData.boundingBox.left + nextWidth)
        {
            stack.push_back(createChildNode(nodeData, 3, firstChild, nextWidth, nextHeight));
        }
    }
}

std::vector<QuadTreeNodeData> QuadTree::findLeaves(const BoundingBox<int32_t>& boundingBox)
{
    return findLeaves(boundingBox, QuadTreeNodeData {
        .depth = 0,
        .boundingBox = _boundingBox,
        .index = 0
    });
}

std::vector<QuadTreeNodeData> QuadTree::findLeaves(const BoundingBox<int32_t>& boundingBox, const QuadTreeNodeData& rootNode)
{
    std::vector<QuadTreeNodeData> leaves;
    std::vector<QuadTreeNodeData> stack;

    stack.push_back(rootNode);

    // We need to traverse the tree to find an appropriate node.
    while (stack.size() > 0)
    {
        auto nodeData = stack.back();
        stack.pop_back();

        auto& node = _nodes[nodeData.index];

        // Ignore objects that do not belong in this quad tree
        if (!boundingBox.intersects(nodeData.boundingBox))
            continue; // Object is out of bounds - skip it

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
            insertChildrenToStack(stack, boundingBox, nodeData, firstChild);
        }
    }

    return leaves;
}

bool QuadTree::insert(uint64_t entityId, const BoundingBox<int32_t>& boundingBox)
{
    // Find leaves that overlap with bounding box - that's the nodes we want to try insert into.
    auto leaves = findLeaves(boundingBox);

    while (leaves.size() > 0)
    {
        auto nodeData = leaves.back();
        leaves.pop_back();

        auto node = _nodes[nodeData.index];

        // Node is leaf, and is full, and node can be divided without dropping below minimum dimensions? If so, subdivide.
        if (node.count >= _capacity && (nodeData.boundingBox.width() >> 1 > _minWidth) && (nodeData.boundingBox.height() >> 1 > _minHeight))
        {
            // Add new child leaf nodes
            _nodes.push_back(QuadTreeNode{
                .firstChildIndex = -1,
                .count = 0
            });
            _nodes.push_back(QuadTreeNode{
                .firstChildIndex = -1,
                .count = 0
            });
            _nodes.push_back(QuadTreeNode{
                .firstChildIndex = -1,
                .count = 0
            });
            _nodes.push_back(QuadTreeNode{
                .firstChildIndex = -1,
                .count = 0
            });

            // Redistribute elements (Remove all elements from this node, add them to the subsequent child nodes)
            if (node.firstChildIndex >= 0)
            {
                // Traverse linked list of elements. firstChildIndex points to the first of those elements.
                uint32_t next = node.firstChildIndex;
                
                // This is now a non-leaf node so point the first child index to the first child of the four created above
                _nodes[nodeData.index].firstChildIndex = (int32_t)(_nodes.size() - 4);
                // reset count
                _nodes[nodeData.index].count = -1;

                do
                {
                   
                    // Add the child nodes to the stack, so that we can check if we should insert the object in any of them
                    insertChildrenToStack(leaves, boundingBox, nodeData, _nodes[nodeData.index].firstChildIndex);
                    
                    // Reinsert elements into child nodes if they overlap with the objects we're moving
                    int32_t nextWidth = nodeData.boundingBox.width() >> 1;
                    int32_t nextHeight = nodeData.boundingBox.height() >> 1;
                    for (auto i = 0; i < 4; i++)
                    {
                        auto childNodeData = createChildNode(nodeData, i, _nodes[nodeData.index].firstChildIndex, nextWidth, nextHeight);
                        auto& childNode = _nodes[childNodeData.index];
                        if (_elements[next].boundingBox.intersects(childNodeData.boundingBox))
                        {
                            // Node is not full, or node cannot be divided? Add object to this node

                            // Change the next pointer to point to the child node's first child element. 
                            _elements[next].next = childNode.firstChildIndex;

                            // We then change the child node's child pointer to point to THIS element, creating a linked list.
                            childNode.firstChildIndex = next;
                            childNode.count++;
                        }
                    }

                    next = _elements[next].next;// todo: redistribute!
                } while (next != -1);
            }
        }
        else 
        {
            // Node is not full, or node cannot be divided? Add object to this node
            _elements.push_back(QuadTreeElement {
                .next = node.firstChildIndex,
                .entityId = entityId,
                .boundingBox = boundingBox
                });
            
            _nodes[nodeData.index].firstChildIndex = (int32_t)(_elements.size() - 1);
            _nodes[nodeData.index].count++;
        }
    }

    return true;
}
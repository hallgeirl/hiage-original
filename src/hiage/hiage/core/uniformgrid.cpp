#include "uniformgrid.hpp"

#include <sstream>
#include <stdexcept>

using namespace hiage;

UniformGrid::UniformGrid() : _initialized(false), _height(0), _width(0), _offsetX(0), _offsetY(0), _gridSize(0), _debugRenderer(nullptr)
{

}

void hiage::UniformGrid::init(const BoundingBox<int32_t>& boundingBox, uint32_t gridSize, DebugRenderer* debugRenderer)
{
	auto width = boundingBox.width();
	auto height = boundingBox.height();

	if (width % gridSize != 0 || height % gridSize != 0)
		throw std::runtime_error("UniformGrid::init: Bounding box dimensions are not divisible by grid size!");

	_width = width / gridSize;
	_height = height / gridSize;
	_offsetX = boundingBox.left;
	_offsetY = boundingBox.bottom;
	_gridSize = gridSize;

	_debugRenderer = debugRenderer;
	_nodes.resize((size_t)_width * _height);
	
	_initialized = true;
}

UniformGrid::GridBoundingBoxResult UniformGrid::getGridBoundingBox(const BoundingBox<int32_t>& boundingBox)
{
	UniformGrid::GridBoundingBoxResult bbr;
	//Figure out the grid cells we need to insert into
	bbr.boundingBox.left = (boundingBox.left + _offsetX) / _gridSize;
	bbr.boundingBox.bottom = (boundingBox.bottom + _offsetY) / _gridSize;
	bbr.boundingBox.right = (boundingBox.right + _offsetX) / _gridSize;
	bbr.boundingBox.top = (boundingBox.top + _offsetY) / _gridSize;

	// Bounds check
	if (bbr.boundingBox.right < 0 || bbr.boundingBox.left >= _width || bbr.boundingBox.top < 0 || bbr.boundingBox.bottom >= _height)
	{
		bbr.success = false;
		return bbr;
	}
	bbr.success = true;

	// Clamp indexes in case the BB is partially outside of the grid
	bbr.boundingBox.left = std::max(bbr.boundingBox.left, 0);
	bbr.boundingBox.right = std::min(bbr.boundingBox.right, _width - 1);
	bbr.boundingBox.bottom = std::max(bbr.boundingBox.bottom, 0);
	bbr.boundingBox.top = std::min(bbr.boundingBox.top, _height - 1);

	return bbr;
}

void UniformGrid::insertNode(int32_t x, int32_t y, int32_t elemIndex, const BoundingBox<int32_t>& boundingBox)
{
	auto index = _elementNodes.insert(GridElementNode{
		.next = -1,
		.index = elemIndex
		});

	auto& node = _nodes[static_cast<size_t>(y * _width + x)];

	if (node.index != -1)
	{
		// Non-empty? Insert it to the beginning and update pointers (constant time). Inserting onto the end of the list requires traversal.
		_elementNodes[index].next = node.index;
	}
	node.index = index;
	
}

// Insert:
// 1. Remove element from grid if already exists
// 2. Find target grid nodes
// 3. Insert element in elements array
// 4. Insert element node that points to this element

bool hiage::UniformGrid::insert(uint64_t entityId, const BoundingBox<int32_t>& boundingBox)
{
	//Figure out the grid cells we need to insert into
	auto boundingBoxResult = getGridBoundingBox(boundingBox);

	if (!boundingBoxResult.success)
		return false;

	auto& bb = boundingBoxResult.boundingBox;
	
	// If already added, remove it first
	remove(entityId);

	auto elemIndex = _elements.insert(GridElement{
		.entityId = entityId,
		.boundingBox = boundingBox
	});
	_elementLookup[entityId] = elemIndex;

	for (int32_t x = bb.left; x <= bb.right; x++)
	{
		for (int32_t y = bb.bottom; y <= bb.top; y++)
		{
			insertNode(x, y, elemIndex, boundingBox);
		}
	}

	return true;
}

std::unordered_set<uint64_t> UniformGrid::getElements(const BoundingBox<int32_t>& boundingBox)
{
	std::unordered_set<uint64_t> result;

	//Figure out the grid cells we need to insert into
	auto boundingBoxResult = getGridBoundingBox(boundingBox);
	if (!boundingBoxResult.success)
		return result;

	auto& bb = boundingBoxResult.boundingBox;

	for (int32_t x = bb.left; x <= bb.right; x++)
	{
		for (int32_t y = bb.bottom; y <= bb.top; y++)
		{
			auto& node = _nodes[static_cast<size_t>(y * _width + x)];
			if (node.index >= -1)
			{
				auto next = node.index;
				do
				{
					auto elNode = _elementNodes[next];
					const auto& el = _elements[elNode.index];
					result.insert(el.entityId);
					next = elNode.next;
				} while (next != -1);
			}
		}
	}

	return result;
}


std::unordered_set<uint64_t> UniformGrid::getElementsNear(uint64_t entityId)
{
	std::unordered_set<uint64_t> result;

	if (!_elementLookup.contains(entityId))
		return result;


	//Figure out the grid cells we need to insert into
	auto element = _elements[_elementLookup.at(entityId)];

	return getElements(element.boundingBox);
}

void UniformGrid::remove(uint64_t entityId)
{
	if (!_elementLookup.contains(entityId))
		return;

	auto index = _elementLookup.at(entityId);
	auto& elem = _elements[index];

	auto& boundingBox = elem.boundingBox;
	
	auto boundingBoxResult = getGridBoundingBox(boundingBox);
	// No need to check success here. Since we're looking at existing elements, we need to assume they are somewhat overlapping with the grid.
	auto& bb = boundingBoxResult.boundingBox;

	for (int32_t x = bb.left; x <= bb.right; x++)
	{
		for (int32_t y = bb.bottom; y <= bb.top; y++)
		{
			auto& node = _nodes[(int32_t)(y * _width + x)];

			auto currentElementNodeIndex = node.index;
			int32_t prevElementNodeIndex = -1;
			do
			{
				auto elNode = _elementNodes[currentElementNodeIndex];
				auto el = _elements[elNode.index];
				if (el.entityId == entityId)
				{
					// Erase the element itself
					_elements.erase(elNode.index);
					
					// Then erase the element node
					if (prevElementNodeIndex == -1)
					{
						// Erasing the first node, so need to update the node's index
						node.index = elNode.next;
					}
					else
					{
						// Erasing a node in the middle, so we must update the previous element node
						_elementNodes[prevElementNodeIndex].next = _elementNodes[currentElementNodeIndex].next;
					}
					_elementNodes.erase(currentElementNodeIndex);
					
					// Drop out of the loop if we found the entity
					currentElementNodeIndex = -1;
				}
				else
				{
					// Go to the next element
					prevElementNodeIndex = currentElementNodeIndex;
					currentElementNodeIndex = elNode.next;
				}
			} while (currentElementNodeIndex != -1);
		}
	}
	_elementLookup.erase(entityId);
}

void UniformGrid::renderDebugInfo()
{
	if (_debugRenderer->enabled() && _debugRenderer->getDebugFlags().uniformGrid.any())
	{
		auto boundingBox = _debugRenderer->getViewPort();

		// Check if viewport is within the grid bounds. If not just exit.
		auto boundingBoxResult = getGridBoundingBox(BoundingBox<int32_t>(boundingBox));
		if (!boundingBoxResult.success)
			return;

		auto& bb = boundingBoxResult.boundingBox;

		std::vector<Vec2d> vertices;
		vertices.resize(4);
		for (int32_t x = bb.left; x <= bb.right; x++)
		{
			for (int32_t y = bb.bottom; y <= bb.top; y++)
			{
				// Draw grid lines
			
				// bottom left
				vertices[0].x = (double)x * _gridSize; vertices[0].y = (double)y * _gridSize;
				// top left
				vertices[1].x = (double)x * _gridSize; vertices[1].y = (double)((double)y + 1) * _gridSize;
				// top right
				vertices[2].x = (double)((double)x + 1) * _gridSize; vertices[2].y = (double)((double)y + 1) * _gridSize;
				// bottom right
				vertices[3].x = (double)((double)x + 1) * _gridSize; vertices[3].y = (double)y * _gridSize;
				_debugRenderer->renderLines(vertices);
				
				// Count nodes and render it
				auto& node = _nodes[static_cast<size_t>((double)y * _width + x)];
				int count = 0;
				if (node.index >= 0)
				{
					auto next = node.index;
					do
					{
						auto elNode = _elementNodes[next];
						const auto& el = _elements[elNode.index];
						count++;
						next = elNode.next;
					} while (next != -1);
				}
				std::stringstream ss;
				ss << count;
				_debugRenderer->renderText(ss.str(), (x * _gridSize + _gridSize / 2), (y * _gridSize + _gridSize / 2));
			}
		}
	}
}
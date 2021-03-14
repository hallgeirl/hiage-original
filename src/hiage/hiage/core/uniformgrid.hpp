#pragma once

#include "boundingbox.hpp"
#include "debugrenderer.hpp"
#include "../util/freelist.hpp"
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace hiage
{
	struct GridNode
	{
		int32_t index = -1; // index points to a location in the elements array
	};

	// Represents the actual elements. There will be only one copy of a GridElement per actual entity in the grid.
	struct GridElement
	{
		uint64_t entityId = 0;
		BoundingBox<int32_t> boundingBox;
	};

	// Represents the linked list of elements in a grid node. These will again point to a GridElement which contains the actual data for an entity.
	struct GridElementNode
	{
		int32_t next = -1;
		int32_t index = -1;
	};

	class UniformGrid
	{
	private:
		struct GridBoundingBoxResult
		{
			BoundingBox<int32_t> boundingBox;
			bool success;
		};

		std::vector<GridNode> _nodes;
		FreeList<GridElementNode> _elementNodes;
		FreeList<GridElement> _elements;
		DebugRenderer* _debugRenderer;

		std::unordered_map<uint64_t, int32_t> _elementLookup; // Lookup index for entity ID => element index
		
		// Height and width are in # of grid cells.
		int32_t _width, _height, _gridSize;
		int32_t _offsetX, _offsetY;
		bool _initialized;

		// Compute the bounding box in grid coordinates from world coordinates
		GridBoundingBoxResult getGridBoundingBox(const BoundingBox<int32_t>& boundingBox);

		void insertNode(int32_t x, int32_t y, int32_t elemIndex, const BoundingBox<int32_t>& boundingBox);
	public:
		/// <summary>
		/// Creates a non-initialized grid. Must call init() before using it, as that's where the allocations happen.
		/// </summary>
		UniformGrid();

		/// <summary>
		/// Allocate the grid. boundingBox dimensions must be divisible by gridSize!
		/// </summary>
		void init(const BoundingBox<int32_t>& boundingBox, uint32_t gridSize, DebugRenderer* debugRenderer);

		bool insert(uint64_t entityId, const BoundingBox<int32_t>& boundingBox);

		void remove(uint64_t entityId);

		std::unordered_set<uint64_t> getElements(const BoundingBox<int32_t>& boundingBox);

		void renderDebugInfo();
		bool initialized() { return _initialized;  }
	};
}
#include "boundingpolygon.hpp"

using namespace std;
using namespace hiage;

hiage::BoundingPolygon::BoundingPolygon() : dirty(false), vertices(), edgeNormals(), left(0), right(0), top(0), bottom(0)
{
}

void hiage::BoundingPolygon::addVertex(const Vector2<double>& vert)
{
	vertices.push_back(vert);
	dirty = true;
}

void hiage::BoundingPolygon::addVertex(double x, double y)
{
	vertices.push_back(Vec2d(x,y));
	dirty = true;
}

void hiage::BoundingPolygon::buildNormals()
{
	edgeNormals.clear();

	auto edgeNormalCount = vertices.size();
	// For 2 vertices, there's 1 edge. For more than 2 vertices, # edges = # vertices.
	if (vertices.size() > 2)
		edgeNormalCount--;

	int left = 0; bottom = 0;
	right = 0; top = 0;

	for (int i = 0; i < edgeNormalCount; i++)
	{
		Vector2<double> edge, normal;

		edge = vertices[i == vertices.size() - 1 ? 0 : i + 1] - vertices[i];
		normal = Vector2<double>(-edge.getY(), edge.getX());
		normal.normalize();

		edgeNormals.push_back(normal);

		//Update outer boundaries
		if (vertices[i].getX() < left) left = i;
		if (vertices[i].getX() > right) right = i;
		if (vertices[i].getY() < bottom) bottom = i;
		if (vertices[i].getY() > top) top = i;
	}

	// Compute the bounds for the last vertex if needed
	for (int i = edgeNormalCount; i < vertices.size(); i++)
	{
		//Update outer boundaries
		if (vertices[i].getX() < left) left = i;
		if (vertices[i].getX() > right) right = i;
		if (vertices[i].getY() < bottom) bottom = i;
		if (vertices[i].getY() > top) top = i;
	}

	dirty = false;

}

void hiage::BoundingPolygon::translate(const Vec2d& distance)
{
	for (auto& vec : vertices)
	{
		vec.add(distance);
	}

	left += distance.getX();
	right += distance.getX();
	top += distance.getY();
	bottom += distance.getY();
}

const std::vector<Vector2<double>>& hiage::BoundingPolygon::getVertices() const
{
	return vertices;
}

const std::vector<Vector2<double>>& hiage::BoundingPolygon::getEdgeNormals() const
{
	if (dirty)
		throw runtime_error("Bounding polygon must be initialized with buildNormals() before normals can be used.");

	return edgeNormals;
}


double hiage::BoundingPolygon::getLeft() const
{
	if (dirty)
		throw runtime_error("Bounding polygon must be initialized with buildNormals() before bounds can be used.");

	return left;
}

double hiage::BoundingPolygon::getRight() const
{
	if (dirty)
		throw runtime_error("Bounding polygon must be initialized with buildNormals() before bounds can be used.");

	return right;
}

double hiage::BoundingPolygon::getTop() const
{
	if (dirty)
		throw runtime_error("Bounding polygon must be initialized with buildNormals() before bounds can be used.");

	return top;
}

double hiage::BoundingPolygon::getBottom() const
{
	if (dirty)
		throw runtime_error("Bounding polygon must be initialized with buildNormals() before bounds can be used.");

	return bottom;
}

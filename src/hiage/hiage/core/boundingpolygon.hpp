#pragma once

#include "../util/vector2.h"

#include <iostream>
#include <vector>

namespace hiage 
{
	class BoundingPolygon
	{
	private:
		std::vector<Vector2<double>> vertices;
		std::vector<Vector2<double>> edgeNormals;
		bool dirty;
		double left, right, bottom, top; // Polygon boundaries
	public:
		BoundingPolygon();
		void addVertex(const Vector2<double>& vert);
		void addVertex(double x, double y);
		void buildNormals();
		void translate(const Vec2d& distance);

		const std::vector<Vector2<double>>& getVertices() const;
		const std::vector<Vector2<double>>& getEdgeNormals() const;

		double getLeft() const;
		double getRight() const;
		double getTop() const;
		double getBottom() const;

	};

	inline std::ostream& operator <<(std::ostream& stream, const BoundingPolygon& r)
	{
		stream << "BoundingPolygon: " << std::endl;
		for (auto& v : r.getVertices())
		{
			stream << "vertex (" << v.getX() << "," << v.getY() << ")" << std::endl;
		}
		return stream;
	}
}
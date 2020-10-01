#pragma once

#include "../util/vector2.h"

#include <algorithm>
#include <vector>

namespace hiage 
{
	/// <summary>
	/// Represents the accumulated result from multiple vector projections.
	/// </summary>
	struct Projection
	{
		double minimum, maximum;

		Projection(double min_, double max_) : minimum(min_), maximum(max_)
		{
		}

		double getLength() const;

		//Return the signed distance between the projections.
		//In case of no overlap: Only one of these are positive, so return that one.
		//In case of overlap: Both are negative, return the one with the smallest magnitude.
		double getDistance(const Projection& p) const;

		bool isOverlapping(const Projection& p) const;
	};

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
		void buildNormals();

		const std::vector<Vector2<double>>& getVertices() const;
		const std::vector<Vector2<double>>& getEdgeNormals() const;

		double getLeft() const;
		double getRight() const;
		double getTop() const;
		double getBottom() const;
	};

	struct CollisionResult
	{
		// Bounding polygons will intersect at t=(0 <= CollisionTime < 1) into the future
		bool hasIntersected = false;
		double collisionTime;

		// Intersecting right now
		bool isIntersecting = false;

		// Pushback vector 
		Vector2<double> minimumTranslationVector;
		Vector2<double> minimumTranslationNormal;

		//Normal for hit surface, and its owner
		Vector2<double> hitNormal;

		//Used internally to determine MTD
		double distance;
	};

	/// <summary>
	/// Class to test for collisions between polygons using the Separating Axis Theorem
	/// </summary>
	class SATCollisionTester
	{
	private:
		// Test one axis with SAT (separating axes theorem).
		// Updates result if there is a "better" hit.
		void testAxis(const Projection& prj1, const Projection& prj2, const Vector2<double>& relativeVelocity, const Vector2<double>& axis, CollisionResult& result, double remainingFrameFraction, int axisOwner);

		// Project a convex polygon onto a vector
		Projection projectPolygon(const BoundingPolygon& p, const Vector2<double>& axis);
	public:
		SATCollisionTester();

		// Test collision against multiple static polygons.
		CollisionResult testCollision(const BoundingPolygon& p, const Vector2<double>& vel, const std::vector<BoundingPolygon>& polygons, int axis);
		
		// Test collision against two moving polygons (usually two objects).
		CollisionResult testCollision(const BoundingPolygon& p1, const Vector2<double>& vel1, const BoundingPolygon& p2, const Vector2<double>& vel2);
	};
}

#include "collisions.hpp"

#include <limits>

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

void hiage::BoundingPolygon::buildNormals()
{
	edgeNormals.clear();
	
	//MoveTo(center.X, center.Y);

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

const std::vector<Vector2<double>>& hiage::BoundingPolygon::getVertices() const
{
	return vertices;
}

const std::vector<Vector2<double>>& hiage::BoundingPolygon::getEdgeNormals() const
{
	return edgeNormals;
}


double hiage::BoundingPolygon::getLeft() const
{
	return left;
}

double hiage::BoundingPolygon::getRight() const
{
	return right;
}

double hiage::BoundingPolygon::getTop() const
{
	return top;
}

double hiage::BoundingPolygon::getBottom() const
{
	return bottom;
}

hiage::SATCollisionTester::SATCollisionTester()
{
}


void hiage::SATCollisionTester::testAxis(const Projection& prj1, const Projection& prj2, const Vector2<double>& relativeVelocity, const Vector2<double>& axis, CollisionResult& result, double remainingFrameFraction, int axisOwner)
{
	bool isIntersecting = false, hasIntersected = false;
	double t = 0; //Collision time

	//Positive distance means we don't have an overlap. Negative means we have an overlap.
	double distance = prj1.getDistance(prj2);

#if DEBUG_COLLISION_OBJECT_POLYGON || DEBUG_COLLISION_OBJECT_OBJECT
	clog << "\tTesting axis " << axis << ", relative vel: " << relativeVelocity << endl;
	clog << "\tProjection 1: " << prj1 << " Projection 2: " << prj2 << endl;
	clog << "\tDistance " << distance << endl;
#endif

	//Already intersecting?
	if (distance < 0)
	{
		isIntersecting = true;
#if DEBUG_COLLISION_OBJECT_POLYGON || DEBUG_COLLISION_OBJECT_OBJECT
		Log.Write("\tIntersecting." << endl;
#endif
	}
	else
	{
		//Calculate velocity component in direction of axis
		double velAxis = axis.dot(relativeVelocity);

#if DEBUG_COLLISION_OBJECT_POLYGON || DEBUG_COLLISION_OBJECT_OBJECT
		clog << "\tNot intersecting. Velocity along axis: " << velAxis << endl;
#endif

		//If projection of polygon 2 is to the right of polygon 1, AND we have a positive velocity along the axis
		//OR projection of polygon 1 is to the left of polygon 2 AND we have a negative velocity along axis
		//then we might have a collision in the future. If not, the objects are either moving in separate directions
		//or they are staying still.
		if ((velAxis > 0 && prj2.minimum >= prj1.maximum) || (velAxis < 0 && prj1.minimum >= prj2.maximum))
		{
			//If the axis belongs to object 1, and it's facing the opposite direction of the velocity,
			//then ignore it because it can't collide. Also, if the axis belongs to object 2,
			//and the axis faces the same direction as the velocity, also ignore it.
#if DEBUG_COLLISION_OBJECT_POLYGON
			clog << "\tAxis dot Velocity: " << axis.DotProduct(relativeVelocity) * (axisOwner == 0 ? -1 : 1) << " Axis: " << axis << endl;
#endif

			//Ignore this test if the axis faces the wrong way
			if (axis.dot(relativeVelocity) * (axisOwner == 0 ? -1 : 1) > 1e-12)
			{
#if DEBUG_COLLISION_OBJECT_POLYGON || DEBUG_COLLISION_OBJECT_OBJECT
				clog << "\tIgnoring test because the edge faces the wrong way. Dot: " << axis.DotProduct(relativeVelocity) * (axisOwner == 0 ? -1 : 1) << "Owner: " + axisOwner;
#endif
				distance = -std::numeric_limits<double>::infinity();
				isIntersecting = true;
			}
			else
			{
				t = distance / abs(velAxis);

				if (t < remainingFrameFraction)
					hasIntersected = true;

#if DEBUG_COLLISION_OBJECT_POLYGON || DEBUG_COLLISION_OBJECT_OBJECT
				clog << "\tCollision time: " + t << endl;
#endif
			}
		}
#if DEBUG_COLLISION_OBJECT_POLYGON || DEBUG_COLLISION_OBJECT_OBJECT
		else
		{
			clog << "\tMoving the wrong way. No collision." << endl;
		}
#endif
	}

	//Find the "best" guess of HOW the objects collides.
	//That is, what direction, and what normal was intersected first.
	if ((!result.isIntersecting && !result.hasIntersected) || //If the result intersection flags are both false, this is the first test.
		(result.isIntersecting && (hasIntersected || (isIntersecting && result.distance < distance))) || //Previous result was an overlapping one, while the latest result indicate o1 and o2 will collide in the future instead,
		(result.hasIntersected && hasIntersected && t > result.collisionTime)) //Previous result was that o1 and o2 collides in the future, but this result indicates that they collide later.
	{
		result.isIntersecting = isIntersecting;
		result.hasIntersected = hasIntersected;
		result.collisionTime = t;
		result.distance = distance;
		result.hitNormal = axis;

#if DEBUG_COLLISION_OBJECT_POLYGON || DEBUG_COLLISION_OBJECT_OBJECT
		clog << "\tNew best axis" << endl;
#endif
	}
	//No intersection now or in the future.
	else if (!isIntersecting && !hasIntersected)
	{
		result.hasIntersected = false;
		result.isIntersecting = false;
	}
}

Projection hiage::SATCollisionTester::projectPolygon(const BoundingPolygon& p, const Vector2<double>& axis)
{
	double minimum = -std::numeric_limits<double>::infinity(), max = std::numeric_limits<double>::infinity();

	for (auto& v : p.getVertices())
	{
		double dot = v.dot(axis);
		if (dot < minimum) minimum = dot;
		if (dot > max) max = dot;
	}

	return Projection(minimum, max);
}

CollisionResult hiage::SATCollisionTester::testCollision(const BoundingPolygon& p, const Vector2<double>& vel, const std::vector<BoundingPolygon>& polygons, int axis)
{
	if (polygons.size() == 0)
	{
		return CollisionResult();
	}
	CollisionResult finalResult = CollisionResult();

	Vec2d velocityFrame;

	if (axis < 0)
		velocityFrame = vel;
	else
	{
		velocityFrame = Vec2d(0, 0);
		if (axis == 0)
			velocityFrame.setX(vel.getX());
		else 
			velocityFrame.setY(vel.getY());
	}


	//Applying Separating Axis theorem
	//First find all the axis. They are the union of the object's edge normals, and the polygon's edge normals.
	//The polygon's edge normals will be retrieved for each polygon that is checked.

	vector<Vector2<double>> edges[2] = { p.getEdgeNormals(), vector<Vector2<double>>() };

#if DEBUG_COLLISION_OBJECT_POLYGON
	int collCount = 0;
	Log.Write("Testing collision object vs polygon, polygon count: " + polygons.Count << endl;
#endif


	//The current minimum time until collision
	double minimumCollisionTime = numeric_limits<double>::infinity();

	//How far have we actually moved this frame?

	//Set the minimum translation vector to the longest vector possible during a frame
	//finalResult.minimumTranslationVector = o.Velocity * frameTime;
	//finalResult.frameTime = frameTime;
//			int finalNormalOwner = -1;

			//Check each edge
	for (const auto& p2 : polygons)
	{
		if (p2.getVertices().size() == 2 && p2.getEdgeNormals()[0].dot(velocityFrame) >= 0)
		{
#if DEBUG_COLLISION_OBJECT_POLYGON
			Log.Write("Polygon has only one edge, which faces the same way as the movement direction. Ignoring." << endl;
#endif
			continue;
		}

#if DEBUG_COLLISION_OBJECT_POLYGON
		Log.Write("Object bounding polygon: " + o.BoundingPolygon << endl;
		Log.Write("Testing polygon " + p << endl;
#endif

		//Collision results for the current polygon
		CollisionResult result;
		result.minimumTranslationVector = velocityFrame;
		edges[1] = p2.getEdgeNormals();

		bool separating = false;
		int normalOwner = -1;

		for (int i = 0; i < 2; i++)
		{
			auto polygonEdges = edges[i];
			for (Vec2d _axis : polygonEdges)
			{
				// Do the collision test on the polygons
				testAxis(projectPolygon(p, _axis), projectPolygon(p2, _axis), velocityFrame, _axis, result, 1, i);
				if (_axis == result.hitNormal)
					normalOwner = i;

				if (!result.hasIntersected && !result.isIntersecting)
				{
					separating = true;
					break;
				}
				if (result.isIntersecting && result.distance == -numeric_limits<double>::infinity()) result.isIntersecting = false;
			}
			if (separating) break;
		}

		//Already intersecting
		if (result.isIntersecting)
		{
			finalResult.isIntersecting = true;
			finalResult.minimumTranslationVector = result.minimumTranslationVector;
			finalResult.minimumTranslationNormal = result.hitNormal * (normalOwner == 0 ? -1 : 1) * abs(result.distance);
		}
		//Will intersect with p in the future. 
		//If we're not already overlapping with another polygon, go ahead and update the current minimum collision time.
		else if (result.hasIntersected)
		{
			//If the collision time is the smallest so far, 
			if (result.collisionTime < minimumCollisionTime)
			{
				minimumCollisionTime = result.collisionTime;
				finalResult.collisionTime = result.collisionTime;
				finalResult.hasIntersected = true;
				finalResult.hitNormal = result.hitNormal;

				if (normalOwner == 0)
					finalResult.hitNormal = finalResult.hitNormal * -1;
			}
		}
	}

	//If we have a first collision, call the collision handler
	if (finalResult.hasIntersected || finalResult.isIntersecting)
	{
#if DEBUG_COLLISION_OBJECT_POLYGON
		clog << "COLLISION." << " Time: " << finalResult.collisionTime << " Normal: " << finalResult.hitNormal << " Remaining: " << remainingFrameTime << " Collision polygon: " << firstCollisionPolygon << " Velocity: " << o.Velocity << " Translation vector: " << finalResult.minimumTranslationVector << endl;
		collCount++;
#endif
	}
	else
	{
#if DEBUG_COLLISION_OBJECT_POLYGON
		clog << "NO COLLISION." << endl;
#endif
	}

#if DEBUG_COLLISION_OBJECT_POLYGON
	clog << "Collision count: " << collCount << "\n" << endl;
#endif

	return finalResult;
}

CollisionResult hiage::SATCollisionTester::testCollision(const BoundingPolygon& p1, const Vector2<double>& vel1, const BoundingPolygon& p2, const Vector2<double>& vel2)
{
#if DEBUG_COLLISION_OBJECT_OBJECT
	clog << "Begin collision test object vs object" << endl
	clog << "Bounding box 1: " + o1.BoundingPolygon.ToString() << endl;
	clog << "Bounding box 2: " + o2.BoundingPolygon.ToString() << endl;
#endif

	//Calculate relative velocity between o1 and o2, as seen from o1
	Vec2d relativeVelocity = (vel1 - vel2);
#if DEBUG_COLLISION_OBJECT_OBJECT
	clog << "Velocity 1 " + o1.Velocity + " Velocity 2 " + o2.Velocity << endl;
#endif
	CollisionResult result;
	bool separating = false;

	vector<Vec2d> polygons[2] = { p1.getEdgeNormals(), p2.getEdgeNormals() };

	// Find each edge normal in the bounding polygons, which is used as axes.
	//foreach (var poly in polygons)
	for (int i = 0; i < 2; i++)
	{
		auto& poly = polygons[i];

		// If the result is ever null, we have a separating axis, and we can cancel the search.
		for (auto& axis : poly)
		{
			//Test for collision on one axis
			testAxis(projectPolygon(p1, axis), projectPolygon(p2, axis), relativeVelocity, axis, result, 1, i);

			//No intersection (now or in the future)
			if (!result.isIntersecting && !result.hasIntersected)
			{
				separating = true;
				break;
			}
		}
		if (separating) break;
	}

	if (!separating)
	{
#if DEBUG_COLLISION_OBJECT_OBJECT
		clog << "COLLISION. Normal: " + result.hitNormal + " Time: " + result.collisionTime << endl;
#endif
		if (result.isIntersecting)
			result.minimumTranslationVector = result.hitNormal * result.distance;
	}
#if DEBUG_COLLISION_OBJECT_OBJECT
	else
	{
		clog << "NO COLLISION" << endl;
	}
	clog << endl;
#endif
	return result;
}

double hiage::Projection::getDistance(const Projection& p) const
{
	return std::max(p.minimum - maximum, minimum - p.maximum);
}

bool hiage::Projection::isOverlapping(const Projection& p) const
{
	return getDistance(p) < 0;
}

double hiage::Projection::getLength() const
{
	return maximum - minimum;
}

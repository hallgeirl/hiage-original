#pragma once

#include "../util/vector2.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace hiage
{
	struct BuiltinEventTypes
	{
		static const int Collision_ObjectTile = 1,
			Collision_ObjectObject = 2;
	};

	class Event
	{
	private:
		int eventType;
	public:
		Event(int eventType) : eventType(eventType) { }
		virtual ~Event() {}

		int getType() const { return eventType; }
	};

	template<class T>
	class GenericEvent : public Event
	{
	private:
		T data;

	protected:
		GenericEvent(int eventType, const T& data) : Event(eventType), data(data) { }
	public:
		const T& getData() const { return data; };
	};

	/*
	Collision events
	*/
	struct ObjectTileCollisionData
	{
		int entityId;
		Vector2<double> objectPosition;
		Vector2<int> tilePosition;
		Vector2<double> normalVector;
	};
	class ObjectTileCollisionEvent : public GenericEvent<ObjectTileCollisionData>
	{
	public:
		ObjectTileCollisionEvent(const ObjectTileCollisionData& theData) : GenericEvent(BuiltinEventTypes::Collision_ObjectTile, theData) {}
	};

	struct ObjectObjectCollisionData
	{
		int entityId1, entityId2;
		Vector2<double> objectPosition1, objectPosition2;
		Vector2<double> normalVector;
	};
	class ObjectObjectCollisionEvent : public GenericEvent<ObjectObjectCollisionData>
	{
	public:
		ObjectObjectCollisionEvent(const ObjectObjectCollisionData& theData) : GenericEvent(BuiltinEventTypes::Collision_ObjectObject, theData) {}
	};

	/*
	Event queue
	*/
	class EventQueue
	{
	private:
		std::unordered_map<int, std::vector<std::unique_ptr<Event>>> events;

	public:
		// NOTE: Enqueuing the event will move it to the event queue - i.e. it can't be accessed again from the caller.
		void enqueue(std::unique_ptr<Event> theEvent);

		std::unique_ptr<Event> dequeue(int eventType);

		std::vector<std::unique_ptr<Event>> dequeueAll();
		void clear();
	};
}
#include "events.hpp"

using namespace hiage;

void EventQueue::enqueue(std::unique_ptr<Event> theEvent)
{
	events[theEvent->getType()].push_back(std::move(theEvent));
}

std::unique_ptr<Event> hiage::EventQueue::dequeue(int eventType)
{
	auto& eventQueue = events[eventType];

	if (eventQueue.empty())
		return nullptr;

	auto evt = std::move(eventQueue.back());
	eventQueue.pop_back();

	return evt;
}

std::vector<std::unique_ptr<Event>> hiage::EventQueue::dequeueAll()
{
	std::vector<std::unique_ptr<Event>> results;

	for (auto& [key, value] : events) 
	{
		for (auto& evt : value)
		{
			results.push_back(std::move(evt));
		}
	}

	return results;
}

const std::vector<std::unique_ptr<Event>>& hiage::EventQueue::peekAll(int eventType)
{
	static std::vector<std::unique_ptr<Event>> empty;
	if (events.contains(eventType))
		return events.at(eventType);
	return empty;
}

void EventQueue::clear()
{
	events.clear();
}

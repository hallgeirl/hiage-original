#pragma once

#include <hiage/core/events.hpp>

struct MarioEventTypes
{
	static const int
		GainCoin = 1000;
};

class GainCoinEvent : public hiage::GenericEvent<int>
{
public:
	GainCoinEvent(const int& theData) : GenericEvent(MarioEventTypes::GainCoin, theData) {}
};
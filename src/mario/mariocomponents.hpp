#pragma once

#include <hiage/core/components.hpp>

struct StompableComponent
{
	int i;
};

struct StomperComponent
{
	int i;
};

struct BlockingComponent
{
	int i;
};

struct GroundMonsterControllerProperties
{
	int i;
};
struct GroundMonsterControllerComponent
{
	std::string direction = "left";
	GroundMonsterControllerComponent() {}
	GroundMonsterControllerComponent(const hiage::ComponentProperties& properties);
};
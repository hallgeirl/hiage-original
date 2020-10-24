#pragma once

#include <hiage/core/components.hpp>

class StompableComponent : public hiage::DatalessComponent<1000>
{
	using DatalessComponent::DatalessComponent;
};

class StomperComponent : public hiage::DatalessComponent<1001>
{
	using DatalessComponent::DatalessComponent;
};

class BlockingComponent : public hiage::DatalessComponent<1002>
{
	using DatalessComponent::DatalessComponent;
};

struct GroundMonsterControllerProperties
{
	std::string direction;
};
class GroundMonsterControllerComponent : public hiage::GenericComponent<GroundMonsterControllerProperties, 1003>
{
public:
	using GenericComponent::GenericComponent;
	virtual GroundMonsterControllerProperties createState(const hiage::ComponentProperties& properties) override;
};
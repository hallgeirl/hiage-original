#pragma once

#include <hiage/core/components.hpp>

class StompableComponent : public hiage::DatalessComponent
{
	using DatalessComponent::DatalessComponent;
};

class StomperComponent : public hiage::DatalessComponent
{
	using DatalessComponent::DatalessComponent;
};

class BlockingComponent : public hiage::DatalessComponent
{
	using DatalessComponent::DatalessComponent;
};

struct GroundMonsterControllerProperties
{
	std::string direction;
};
class GroundMonsterControllerComponent : public hiage::GenericComponent<GroundMonsterControllerProperties>
{
public:
	using GenericComponent::GenericComponent;
	virtual GroundMonsterControllerProperties createState(const hiage::ComponentProperties& properties) override;
};
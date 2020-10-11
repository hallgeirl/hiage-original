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
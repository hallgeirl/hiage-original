#pragma once

#include <unordered_map>
#include <string>
#include <variant>

namespace hiage
{
	/*
	Data structures to describe objects and other resources, acting as "blueprints" for creating them.
	*/

	class ComponentProperties : public std::unordered_map<std::string, std::variant<std::string, double, std::vector<std::string>, std::vector<double>>>
	{
	public:
		ComponentProperties() {}


	};

	struct ComponentDescriptor
	{
        std::string type;
		ComponentProperties properties;
	};

	struct ObjectDescriptor
	{
		std::string name, type;
        std::vector<ComponentDescriptor> components;
	};
}
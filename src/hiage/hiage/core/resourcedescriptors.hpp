#pragma once

#include <unordered_map>
#include <string>
#include <variant>

namespace hiage
{
	/*
	Data structures to describe objects and other resources, acting as "blueprints" for creating them.
	*/

	class ComponentProperties : public std::unordered_map<std::string, std::variant<std::string, double>>
	{};

	struct ComponentDescriptor
	{
        std::string type;
        std::unordered_map<std::string, std::variant<std::string, double>> properties; 
	};

	struct ObjectDescriptor
	{
		std::string name, type;
        std::vector<ComponentDescriptor> components;
	};
}
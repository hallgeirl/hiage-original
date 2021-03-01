#pragma once

#include <vector>
#include <string>
#include <json/json.hpp>

namespace hiage
{
	class JsonHelpers
	{
	public:
		static std::vector<std::string> parseStringArray(const nlohmann::json& jsonObj)
		{
			std::vector<std::string> values;
			for (auto& v : jsonObj)
			{
				values.push_back(v);
			}

			return values;
		}

		static std::vector<double> parseNumberArray(const nlohmann::json& jsonObj)
		{
			std::vector<double> values;
			for (auto& v : jsonObj)
			{
				values.push_back(v);
			}

			return values;
		}
	};
}
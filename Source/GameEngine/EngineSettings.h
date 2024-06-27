#pragma once
#include <filesystem>
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
namespace nl = nlohmann;

namespace EngineSettings
{
	static const std::filesystem::path GetContentRootPath()
	{
		std::filesystem::path exeDir = std::filesystem::current_path();
		std::ifstream path(exeDir / "ApplicationSettings.json");
		nl::json data = nl::json();

		try
		{
			data = nl::json::parse(path);
		}
		catch (nl::json::parse_error e)
		{
			std::cout << e.what();
			return std::filesystem::path();
		}
		path.close();

		std::filesystem::path assetsDir = exeDir / data["assetsDir"].get<std::string>();
		return assetsDir;
	}
}
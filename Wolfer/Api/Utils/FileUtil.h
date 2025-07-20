#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace FileUtil {

	inline std::string getRoamingStatePath() {
		static std::string RoamingStatePath = (getenv("AppData") + std::string("\\..\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState\\"));
		return RoamingStatePath;
	}

	inline std::string getClientPath() {
		static std::string ClientPath = getRoamingStatePath() + "Wolfer\\";
		return ClientPath;
	}

	inline bool doesFilePathExist(const std::string& path) {
		return std::filesystem::exists(path);
	}

	inline bool createPath(const std::string& path) {
		return std::filesystem::create_directory(path);
	}

	inline bool deletePath(const std::string& path) {
		return std::filesystem::remove(path);
	}
}

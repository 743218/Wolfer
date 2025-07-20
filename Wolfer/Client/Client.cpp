#include "Client.h"

#include "../Api/Utils/FileUtil.h"
#include "../Api/Utils/NetworkUtil.h"
#include "../Api/Utils/Logger.h"

void AddFont(const std::string& fontPath) {
	std::wstring temp = std::wstring(fontPath.begin(), fontPath.end());
	AddFontResource(temp.c_str());
}

void Client::DisplayClientMessage(const char* fmt, ...) {
	LocalPlayer* localPlayer = g_Data.getLocalPlayer();
	if (localPlayer == nullptr)
		return;

	va_list arg;
	va_start(arg, fmt);
	char message[300];
	vsprintf_s(message, 300, fmt, arg);
	va_end(arg);

	static std::string headerMessage;
	if (headerMessage.empty()) {
		char headerCStr[50];
		sprintf_s(headerCStr, 50, "%s%s%s%s %s", MCTF::DARK_GRAY, MCTF::RED, "Wolfer", MCTF::DARK_GRAY, MCTF::WHITE);
		headerMessage = std::string(headerCStr);
	}
	std::string messageStr = headerMessage + std::string(message);
	localPlayer->displayClientMessage(messageStr);
}

void Client::init() {

	std::string ClientPath = FileUtil::getClientPath();
	if (!FileUtil::doesFilePathExist(ClientPath)) {
		FileUtil::createPath(ClientPath);
	}

	Logger::init();
	ConfigManager::init();
	
	Addresses::init();
	MCTF::initMCTF();

	ModuleManager::init();
	CommandManager::init();
	HookManager::init();

	if (g_Data.clientInstance == nullptr)
		return;
	/*
	logF("%llx", g_Data.clientInstance);
	logF("%llx", g_Data.clientInstance->mcGame);
	logF("%llx", g_Data.getLocalPlayer());
	*/

	initialized = true;

	Beep(50, 255);
	NotificationManager::addNotification("Successfully injected Wolfer", 5.f);

	if (!ConfigManager::doesConfigExist("default"))
		ConfigManager::createNewConfig("default");

	ConfigManager::loadConfig("default");
}

void Client::shutdown() {
	ConfigManager::saveConfig();
	initialized = false;
}
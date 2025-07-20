#pragma once

#include <string>
#include <fstream>
#include <Windows.h>
#include <wininet.h>

namespace NetworkUtil {

	inline bool canAccessInternet() {
		char url[] = "https://www.google.com/";
		return InternetCheckConnectionA(url, FLAG_ICC_FORCE_CONNECTION, 0);
	}

	inline bool downloadFile(std::string name, std::string path, std::string url) {
		std::string filePath = path + name;

		HINTERNET webH = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
		if (!webH)
			return false;

		HINTERNET urlFile = InternetOpenUrlA(webH, url.c_str(), NULL, NULL, NULL, NULL);
		if (!urlFile) {
			InternetCloseHandle(webH);
			return false;
		}

		std::ofstream outputFile(filePath, std::ios::binary);
		if (!outputFile.is_open()) {
			InternetCloseHandle(webH);
			InternetCloseHandle(urlFile);
			return false;
		}

		char buffer[2000];
		DWORD bytesRead;

		do {
			InternetReadFile(urlFile, buffer, sizeof(buffer), &bytesRead);
			outputFile.write(buffer, bytesRead);
			memset(buffer, 0, sizeof(buffer));
		} while (bytesRead);

		outputFile.close();
		InternetCloseHandle(webH);
		InternetCloseHandle(urlFile);

		return true;
	}
}

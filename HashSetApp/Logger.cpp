#include "pch.h"
#include "Logger.h"
#include <fstream>
#include <ctime>
#include <iomanip>
#include <locale>
#include <codecvt>

void Logger::LogInfo(const std::string& message) {
    WriteLog("INFO", message);
}

void Logger::LogWarning(const std::string& message) {
    WriteLog("WARNING", message);
}

void Logger::LogError(const std::string& message) {
    WriteLog("ERROR", message);
}

std::string ConvertToUTF8(const std::string& str) {
    if (str.empty()) return str;

    int wsize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), NULL, 0);
    if (wsize == 0) return str;

    std::wstring wstr(wsize, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), &wstr[0], wsize);

    int utf8size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    if (utf8size == 0) return str;

    std::string utf8str(utf8size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &utf8str[0], utf8size, NULL, NULL);

    return utf8str;
}

void Logger::WriteLog(const std::string& type, const std::string& message) {
    std::ofstream logFile("HASHLOG.txt", std::ios::app | std::ios::binary);
    if (logFile.is_open()) {
        if (logFile.tellp() == 0) {
            const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
            logFile.write(reinterpret_cast<const char*>(bom), sizeof(bom));
        }

        std::string timeUtf8 = ConvertToUTF8(GetCurrentTime());
        std::string typeUtf8 = ConvertToUTF8(type);
        std::string messageUtf8 = ConvertToUTF8(message);

        logFile << "[" << timeUtf8 << "] [" << typeUtf8 << "] " << messageUtf8 << "\n";
        logFile.close();
    }
}

std::string Logger::GetCurrentTime() {
    auto now = std::time(nullptr);
    struct tm newtime;
    localtime_s(&newtime, &now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &newtime);
    return buffer;
}
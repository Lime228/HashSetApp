#pragma once
#include <string>

class Logger {
public:
    static void LogInfo(const std::string& message);
    static void LogWarning(const std::string& message);
    static void LogError(const std::string& message);

private:
    static std::string GetCurrentTime();
    static void WriteLog(const std::string& type, const std::string& message);
};
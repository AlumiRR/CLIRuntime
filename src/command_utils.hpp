#pragma once

#include <string>

namespace cliutils // Нет смысла объединять это все в объект
{
    enum Command
    {
        STOP,
        DIR,
        CUSTOM,
        UNKNOWN
    };
    std::string eraseFirstWord(std::string str);
    std::string eraseEverythingExceptFirstWord(const std::string str);
    void removeQuotations(std::string &str);
    std::string listDir(std::string path);
    double getUptime();
    Command parseCorruptedString(const std::string str);
    Command parseString(std::string str);
    std::string executeCommand(Command comm, std::string arg = "");
    std::string explainCommand(Command comm, std::string arg = "");
}
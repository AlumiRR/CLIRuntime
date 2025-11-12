#pragma once

#include <string>
#include <sstream>
#include <filesystem>

using namespace std;

namespace cliutils // Нет смысла объединять это все в объект
{
    enum Command
    {
        STOP,
        DIR,
        CUSTOM,
        UNKNOWN
    };

    string eraseFirstWord(string str)
    {
        string delimiter = " ";
        str = str.erase(0, str.find(delimiter) + delimiter.length());
        return str;
    };

    string eraseEverythingExceptFirstWord(const string str)
    {
        istringstream iss(str);
        string firstWord;

        if (iss >> firstWord)
        {
            return firstWord;
        }

        return "";
    };

    void remove_quotations(string &str)
    {
        if (str.front() == '\"' && str.back() == '\"')
        {
            str.erase(0);
            str.pop_back();
        }
    }

    string listDir(string path)
    {
        string listing;
        try
        {
            for (const auto &entry : filesystem::directory_iterator(path))
            {
                listing += string(entry.path().extension()) + " " + string(entry.path().filename()) + "\n";
            }
        }
        catch (const filesystem::filesystem_error& e)
        {
            return e.what();
        }

        return listing;
    };

    double getUptime()
    {
        std::ifstream uptime_file("/proc/uptime");
        if (!uptime_file.is_open())
        {
            throw runtime_error("Couldn't open /proc/uptime");
            return -1.0;
        }

        double uptimeSeconds;
        uptime_file >> uptimeSeconds;
        uptime_file.close();
        return uptimeSeconds;
    };

    Command parseCorruptedString(const string str)
    {
        if (str.find("stop") == 0)
            return STOP;
        else if (str.find("listen") || str.find("list") || str.find("ls") || str.find("dir"))
            return DIR;
        return UNKNOWN;
    };

    Command parseString(string str)
    {
        str = eraseEverythingExceptFirstWord(str);
        if (str == "stop")
        {
            return STOP;
        }
        else if (str == "listen" || str == "list" || str == "ls" || str == "dir")
        {
            return DIR;
        }
        return parseCorruptedString(str);
    };

    string executeCommand(Command comm, string arg = "") 
    {
        arg = eraseEverythingExceptFirstWord(arg);
        switch (comm)
        {
        case STOP:
            return "Terminating..."; // exit() вне этой функции чтобы успеть отправить ответ
            break;
        case DIR:
            return listDir(arg);
            break;
        default:
            return "Error: Unknown command";
            break;
        }
    };

    string explainCommand(Command comm, string arg = "")
    {
        switch (comm)
        {
        case STOP:
            return "stop";
            break;
        
        default:
            return "unknown command";
            break;
        }
    };
}
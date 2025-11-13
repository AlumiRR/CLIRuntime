#include "command_utils.hpp"

#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>

using namespace std;

string cliutils::eraseFirstWord(string str)
{
    string delimiter = " ";
    str = str.erase(0, str.find(delimiter) + delimiter.length());
    return str;
}

string cliutils::eraseEverythingExceptFirstWord(const string str)
{
    istringstream iss(str);
    string firstWord;

    if (iss >> firstWord)
    {
        return firstWord;
    }

    return "";
}

void cliutils::removeQuotations(string &str)
{
    if (str.front() == '\"' && str.back() == '\"')
    {
        str.erase(0, 1);
        str.pop_back();
    }
}

string cliutils::listDir(string path)
{
    string listing;
    try
    {
        for (const auto &entry : filesystem::directory_iterator(path))
        {
            listing += string(entry.path().extension()) + " " + string(entry.path().filename()) + "\n";
        }
    }
    catch (const filesystem::filesystem_error &e)
    {
        return e.what();
    }

    return listing;
}

double cliutils::getUptime()
{
    ifstream uptime_file("/proc/uptime");
    if (!uptime_file.is_open())
    {
        throw runtime_error("Couldn't open /proc/uptime");
        return -1.0;
    }

    double uptimeSeconds;
    uptime_file >> uptimeSeconds;
    uptime_file.close();
    return uptimeSeconds;
}

cliutils::Command cliutils::parseCorruptedString(const string str)
{
    if (str.find("stop") == 0)
        return STOP;
    else if (str.find("listen") == 0 || str.find("list") == 0 || str.find("ls") == 0 || str.find("dir") == 0)
        return DIR;
    return UNKNOWN;
}

cliutils::Command cliutils::parseString(string str)
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
}

string cliutils::executeCommand(Command comm, string arg)
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
}

string cliutils::explainCommand(Command comm, string arg)
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
}
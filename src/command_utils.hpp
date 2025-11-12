#pragma once

#include <string>

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

    Command parseCorruptedString(const string str)
    {
        if (str.find("stop") == 0)
        {
            return STOP;
        }
        return UNKNOWN;
    };

    Command parseString(const string str)
    {
        if (str == "stop")
        {
            return STOP;
        }
        return parseCorruptedString(str);
    };

    string executeCommand(Command comm, string arg = "") 
    {

        switch (comm)
        {
        case STOP:
            return "Terminating..."; // exit() вне этой функции чтобы успеть отправить ответ
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

    string eraseFirstWord(string str)
    {
        string delimiter = " ";
        str = str.erase(0, str.find(delimiter) + delimiter.length());
        return str;
    };
}
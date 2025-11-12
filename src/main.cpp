#include <iostream>
#include <string>

#include "daemon.hpp"

#include "defines.hpp"
#include "udp.hpp"
#include "command_utils.hpp"

using namespace daemonpp;
using namespace std::chrono_literals;

using namespace std;

// --- Сервис ---

class CLIDaemon : public daemon // Линковщик жалуется если не здесь :(
{
    public:
        void on_start(const dconfig &cfg) override
        {
            dlog::info("CLIDaemon version: " + cfg.get("version") + " started");

            dlog::info("Starting UDP server...");
            try
            {
                socket.setReuseAddress(true);

                string port_str = cfg.get("port");
                dlog::info("Port in conf: " + port_str); // Не работает

                if (port_str.length() > 0)
                {
                    port = stoi(port_str);
                }
                else
                {
                    port = PORT;
                }

                socket.bind("0.0.0.0", port);
                socket.setTimeout(3);
            }
            catch(const std::exception& e)
            {
                dlog::critical("Couldn't set up UDP socket: " + string(e.what()));
                stop(EXIT_FAILURE);
            }
        
            dlog::info("UDP server started on port " + to_string(port));
        };
        void on_update() override
        {
            dlog::info("Checking for UDP requests");

            // --- Ждем сообщений ---

            socket.setTimeout(60);

            string source_addr;
            uint16_t source_port;
            char buffer[1024];
            ssize_t received = socket.receiveFrom(buffer, sizeof(buffer), source_addr, source_port);

            // --- Парсим сообщение-комманду ---

            cliutils::Command cmd;
            cmd = cliutils::parseString(string(buffer));

            dlog::info("Recieved command: " + string(buffer) + " (" + cliutils::explainCommand(cmd) + ")");

            // --- Исполняем комманду ---

            string response = cliutils::executeCommand(cmd, cliutils::eraseFirstWord(string(buffer)));

            // --- Отправляем ответ ---

            socket.setTimeout(3);
            socket.sendTo(response.data(), response.size(), source_addr, source_port);

            // --- Останавливаем сервис если такова комманда ---

            if (cmd == cliutils::STOP)
            {
                stop(EXIT_SUCCESS);
            }
        };
        void on_stop() override
        {
            dlog::info("Terminating...");
        };
        void on_reload(const dconfig &cfg) override
        {
            dlog::info("my_daemon::on_reload(): new daemon version from updated config: " + cfg.get("version"));
        };

    private:
        UDPSocket socket;
        uint16_t port;
};

void remove_quotations(string &str)
{
    if (str.front() == '\"' && str.back() == '\"')
    {
        str.erase(0);
        str.pop_back();
    }
}

int main(int argc, const char *argv[])
{
    string command_string;
    uint16_t server_port;

    // --- Парсинг аргументов ---

    for (short i = 0; i < argc; i++)
    {
        /// --- Версия ---
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            cout << "CLIRuntime " << VERSION << endl;
            return EXIT_SUCCESS;
        }
        /// --- Комманда серверу ---
        else if (strcmp(argv[i], "-c") == 0)
        {
            try
            {
                command_string = (string)argv[i + 1];
            }
            catch (const exception &e)
            {
                cerr << "Не указана комманда после аргумента -c" << endl;
                return EXIT_FAILURE;
            }

            for (int j = i + 2; j < argc; j++)
            {
                string str = (string)argv[j];
                if (str.front() == '-')
                    break;
                command_string += " " + str;
                if (str.back() == '\"')
                    break;
            }
        }
        else if (strstr(argv[i], "--cmd=") != NULL) // TODO Заменить на более строгую проверку
        {
            command_string = (string)argv[i];
            for(int j = i + 1; j < argc; j++)
            {
                string str = (string)argv[j];
                if(str.front() == '-')
                    break;
                command_string += " " + str;
                if(str.back() == '\"')
                    break;
            }

            command_string.replace(0, 6, "");
            if (command_string.length() < 1)
            {
                cerr << "Не указана комманда после аргумента --cmd=" << endl;
                return EXIT_FAILURE;
            }
            //cout << "CLIRuntime " << command_string << endl;
            //return EXIT_SUCCESS;
        } 
        /// --- Порт ---
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0)
        {
            string server_port_str;
            try
            {
                server_port_str = (string)argv[i + 1]; 
            }
            catch (const exception &e)
            {
                cerr << "Не указан порт после аргумента --port / -p" << endl;
                return EXIT_FAILURE;
            }
            server_port = stoi(server_port_str);
        }
    }
    
    remove_quotations(command_string);

    // --- Обмен сообщениями между сервером и клиентом ---

    if (command_string.length() > 0)
    {
        UDPSocket client;

        cout << "<< " << command_string << endl;

        command_string += " ___"; // Буфер для мусора от UDP

        
        // --- Отправляем сообщение ---
        client.setTimeout(3);
        client.sendTo(command_string.data(), command_string.size(), LOCALHOST, server_port);

        /// --- Ожидаем ответ ---
        string server_addr;
        uint16_t server_port;
        char buffer[1024];
        ssize_t received = client.receiveFrom(buffer, sizeof(buffer), server_addr, server_port);

        // --- Выводим ответ ---
        if (received >= 0)
        {
            cout << ">> " << string(buffer) << endl;
        }
        else
        {
            cout << "!< " << "No response" << endl;
            cerr << "Couldn't reach server" << endl;
            return EXIT_FAILURE;
        }
        
        return EXIT_SUCCESS;
    }

    // --- Запуск сервиса ---

    cout << "Starting CLIDaemon" << endl;

    CLIDaemon dmn;
    dmn.set_name("CLIDaemon"); // Название
    dmn.set_update_duration(10ms); // Цикл каждые ... едениц времени (все равно спит ожидая сообщение)
    dmn.set_cwd("/"); // Рабочая директория
    dmn.run(argc, argv);

    return EXIT_SUCCESS;
}
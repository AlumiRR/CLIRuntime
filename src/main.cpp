#include <iostream>
#include <string>

#include "daemon.hpp"

#include "defines.hpp"
#include "udp.hpp"

using namespace daemonpp;
using namespace std::chrono_literals;

class CLIDaemon : public daemon
{
    public:
        void on_start(const dconfig &cfg) override
        {
            dlog::info("CLIDaemon version: " + cfg.get("version") + " started");

            dlog::info("Starting UDP server...");
            socket.setReuseAddress(true);
            socket.bind("0.0.0.0", PORT);
            dlog::info("UDP server started");
        };
        void on_update() override
        {
            dlog::info("Checking for UDP requests");

            std::string source_addr;
            uint16_t source_port;
            char buffer[1024];
            ssize_t received = socket.receiveFrom(buffer, sizeof(buffer), source_addr, source_port);
            dlog::info("Received: " + std::string(buffer));
        };
        void on_stop() override
        {
            /// Called once before daemon is about to exit with system shutdown or when you manually call `$ systemctl stop my_daemon`
            /// Cleanup your code here...

            dlog::info("my_daemon::on_stop()");
        };
        void on_reload(const dconfig &cfg) override
        {
            /// Called once after your daemon's config fil is updated then reloaded with `$ systemctl reload my_daemon`
            /// Handle your config updates here...

            dlog::info("my_daemon::on_reload(): new daemon version from updated config: " + cfg.get("version"));
        };

    private:
        UDPSocket socket;
};

using namespace std;

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

    // --- Парсинг аргументов ---

    for (short i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            cout << "CLIRuntime " << VERSION << endl;
            return EXIT_SUCCESS;
        }
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

            //cout << "CLIRuntime " << command_string << endl;
            //return EXIT_FAILURE;
        }
        else if (strstr(argv[i], "--cmd=") != NULL) // TODO Заменить на более строгую проверку
        {
            command_string = (string)argv[i];

            command_string.replace(0, 6, "");
            if (command_string.length() < 1)
            {
                cerr << "Не указана комманда после аргумента --cmd=" << endl;
                return EXIT_FAILURE;
            }
            //cout << "CLIRuntime " << command_string << endl;
            //return EXIT_SUCCESS;
        }
    }
    
    remove_quotations(command_string);

    if (command_string.length() > 0)
    {
        UDPSocket client;
        client.sendTo(command_string.data(), command_string.size(), LOCALHOST, PORT);
        return EXIT_SUCCESS;
    }

    cout << "Starting CLIDaemon" << endl;
    CLIDaemon dmn;                             // create a daemon instance
    dmn.set_name("CLIDaemon");     // set daemon name to identify logs in syslog
    dmn.set_update_duration(3s); // set duration to sleep before triggering the on_update callback 3 seconds
    dmn.set_cwd("/");                        // set daemon's current working directory to root /
    dmn.run(argc, argv);                 // run your daemon

    cout << "Daemon started" << endl;
    return EXIT_SUCCESS;
}
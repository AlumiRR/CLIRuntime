#include <iostream>

#include "daemon.hpp"

#include "defines.hpp"

using namespace daemonpp;
using namespace std::chrono_literals;

class CLIDaemon : public daemon
{
public:
    void on_start(const dconfig &cfg) override
    {
        dlog::info("my_daemon::on_start(): my_daemon version: " + cfg.get("version") + " started successfully!");
    };
    void on_update() override
    {
        /// Called every DURATION set in set_update_duration()...
        /// Update your code here...

        dlog::info("my_daemon::on_update()");
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
};

using namespace std;

int main(int argc, const char* argv[]) {

  // --- Парсинг аргументов ---

  for (short i = 0; i < argc; i++)
  {
    if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
    {
      cout << "CLIRuntime " << VERSION << endl;
      return EXIT_SUCCESS;
    }
  }


  
  CLIDaemon dmn;                         // create a daemon instance
  dmn.set_name("CLIDaemon"); // set daemon name to identify logs in syslog
  dmn.set_update_duration(3s);   // set duration to sleep before triggering the on_update callback 3 seconds
  dmn.set_cwd("/");      // set daemon's current working directory to root /
  dmn.run(argc, argv);                   // run your daemon
  return EXIT_SUCCESS;
}
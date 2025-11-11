#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <cstring>
#include <system_error>

//#include "interfaces.hpp"

#define PORT 7070
#define MAXLINE 1024
#define LOCALHOST "127.0.0.1"

enum Command
{
    STOP,
    DIR_LIST
};

class ReqParser
{
    public:
        virtual Command dataToCommand(void* data, int len) = 0;
        virtual void* commandToData(Command command, int &len) = 0;
};

class Raw_ReqParser : public ReqParser
{
    public:
        Command dataToCommand(void* data, int len) override;
        void* commandToData(Command command, int &len) override;
};

class UDPSocket
{
    public:
        UDPSocket();
        void bind(const std::string& address, uint16_t port);
        ssize_t sendTo(const void* buffer, size_t length, const std::string& address, uint16_t port);
        ssize_t receiveFrom(void* buffer, size_t length, std::string& address, uint16_t& port);
        void setReuseAddress(bool enable);
        void setBlocking(bool enable);
        ~UDPSocket();
    
    protected:
        int descriptor = -1;
};
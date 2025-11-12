#include "udp.hpp"

using namespace std;

UDPSocket::UDPSocket()
{
    descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (descriptor < 0)
    {
        //clir_log("Could not create socket");
        throw std::system_error(errno, std::system_category(), "Could not create socket");
    }
}

UDPSocket::~UDPSocket()
{
    if (descriptor >= 0)
    {
        close(descriptor);
    }
}

void UDPSocket::bind(const std::string &address, uint16_t port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, address.c_str(), &addr.sin_addr) <= 0)
    {
        //clir_log("Invalid address during binding");
        throw std::system_error(errno, std::system_category(), "Invalid address during binding");
    }

    if (::bind(descriptor, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        //clir_log("Couldn't bind adress");
        throw std::system_error(errno, std::system_category(), "Couldn't bind adress");
    }
}

ssize_t UDPSocket::sendTo(const void* buffer, size_t length, const std::string& address, uint16_t port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, address.c_str(), &addr.sin_addr) <= 0)
    {
        //clir_log("Wrong address during sending");
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return -1; // Timeout occurred
        }
        throw std::system_error(errno, std::system_category(), "Wrong address during sending");
    }

    ssize_t sent = sendto(descriptor, buffer, length, 0, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (sent < 0)
    {
        //clir_log("Couldn't send data");
        throw std::system_error(errno, std::system_category(), "Couldn't send data");
    }
    return sent;
}

ssize_t UDPSocket::receiveFrom(void* buffer, size_t length, std::string& address, uint16_t& port)
{
    sockaddr_in addr{};
    socklen_t addr_len = sizeof(addr);

    ssize_t received = recvfrom(descriptor, buffer, length, 0, reinterpret_cast<sockaddr *>(&addr), &addr_len);
    if (received < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return -1; // Timeout occurred
        }
        throw std::system_error(errno, std::system_category(), "Couldn't read data during receiving");
    }

    char ip_str[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN) == nullptr)
    {
        //clir_log("Wrong address during receiving");
        throw std::system_error(errno, std::system_category(), "Wrong address during receiving");
    }
    address = ip_str;
    port = ntohs(addr.sin_port);
    return received;
}

void UDPSocket::setReuseAddress(bool enable)
{
    int value = enable ? 1 : 0;
    if (setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) < 0)
    {
        //clir_log("Couldn't set socket options");
        throw std::system_error(errno, std::system_category(), "Couldn't set socket options");
    }
}

void UDPSocket::setBlocking(bool enable)
{
    int flags = fcntl(descriptor, F_GETFL, 0);
    if (flags < 0)
    {
        //clir_log("Couldn't get net flags");
        throw std::system_error(errno, std::system_category(), "Couldn't get net flags");
    }

    flags = enable ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    if (fcntl(descriptor, F_SETFL, flags) < 0)
    {
        //clir_log("Couldn't set net flags");
        throw std::system_error(errno, std::system_category(), "Couldn't set net flags");
    }
}

void UDPSocket::setTimeout(int seconds)
{
    timeval timeout{};
    timeout.tv_sec = seconds;

    if (setsockopt(descriptor, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        throw std::system_error(errno, std::system_category(), "Couldn't set UDP receiving timeout");
    }

    if (setsockopt(descriptor, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        throw std::system_error(errno, std::system_category(), "Couldn't set UDP sending timeout");
    }
}
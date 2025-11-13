#include <gtest/gtest.h>

#include <vector>
#include <string>
#include "udp.hpp"

using namespace std;

class UDPTest : public ::testing::Test {
protected:
    UDPSocket alice;
    uint16_t alice_port;

    UDPSocket bob;
    uint16_t bob_port;

    void SetUp() override {
        alice.setReuseAddress(true);
        bob.setReuseAddress(true);

        alice_port = 7001;
        bob_port = 7002;

        alice.bind("0.0.0.0", alice_port);
        bob.bind("0.0.0.0", bob_port);
        
        alice.setTimeout(3);
        bob.setTimeout(3);
    }
    
    void TearDown() override {
        alice = UDPSocket();
        bob = UDPSocket();
    }
};

TEST_F(UDPTest, simpleMessaging)
{
    string hi_alice = "Hi Alice! ___";
    string hi_bob = "Hi Bob! ___";

    string bob_source_addr;
    uint16_t bob_source_port;
    char bob_buffer[1024];

    alice.sendTo(hi_bob.data(), hi_bob.size(), LOCALHOST, bob_port);
    ssize_t bob_received = bob.receiveFrom(bob_buffer, sizeof(bob_buffer), bob_source_addr, bob_source_port);

    string bob_heard = (string) bob_buffer;

    EXPECT_TRUE(bob_heard.find(hi_bob) != string::npos);

    string alice_source_addr;
    uint16_t alice_source_port;
    char alice_buffer[1024];

    bob.sendTo(hi_alice.data(), hi_alice.size(), LOCALHOST, alice_port);
    ssize_t alice_received = alice.receiveFrom(alice_buffer, sizeof(alice_buffer), alice_source_addr, alice_source_port);

    string alice_heard = (string) alice_buffer;

    EXPECT_TRUE(alice_heard.find(hi_alice) != string::npos);

    EXPECT_EQ(alice_source_port, bob_port);
    EXPECT_EQ(bob_source_port, alice_port);
}
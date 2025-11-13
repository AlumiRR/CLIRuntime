#include <gtest/gtest.h>

#include <vector>
#include <string>
#include "command_utils.hpp"

using namespace std;

TEST(ComutilsTest, eraseFirstWord)
{
    EXPECT_EQ(cliutils::eraseFirstWord("hello hi uhmmm"), "hi uhmmm");
}

TEST(ComutilsTest, eraseEverythingExceptFirstWord)
{
    EXPECT_EQ(cliutils::eraseEverythingExceptFirstWord("hello hi uhmmm"), "hello");
}

TEST(ComutilsTest, removeQuotations)
{
    string str = "\"hello\"";
    cliutils::removeQuotations(str);
    EXPECT_EQ(str, "hello");

    str = "hello";
    cliutils::removeQuotations(str);
    EXPECT_EQ(str, "hello");

    str = "";
    cliutils::removeQuotations(str);
    EXPECT_EQ(str, "");

    str = "\"\"\"hello\"\"\"";
    cliutils::removeQuotations(str);
    EXPECT_EQ(str, "\"\"hello\"\"");
}

TEST(ComutilsTest, listDir)
{
    string str = cliutils::listDir("/");
    EXPECT_TRUE(str.find("root") != string::npos);
}

TEST(ComutilsTest, getUptime)
{
    EXPECT_GT(cliutils::getUptime(), 0.0);
}

TEST(ComutilsTest, parseString)
{
    EXPECT_EQ(cliutils::parseString("stop"), cliutils::Command::STOP);
    EXPECT_EQ(cliutils::parseString("stop     "), cliutils::Command::STOP);
    EXPECT_EQ(cliutils::parseString("stop1313"), cliutils::Command::STOP);
    EXPECT_EQ(cliutils::parseString("listen"), cliutils::Command::DIR);
    EXPECT_EQ(cliutils::parseString("dir"), cliutils::Command::DIR);
    EXPECT_EQ(cliutils::parseString("hello"), cliutils::Command::UNKNOWN);
}

TEST(ComutilsTest, executeCommand)
{
    EXPECT_EQ(cliutils::executeCommand(cliutils::Command::DIR, "/"), cliutils::listDir("/"));
}
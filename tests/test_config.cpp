#include <gtest/gtest.h>
#include "../include/Config.hpp"
#include "../include/Utils.hpp"
#include "../include/Error.hpp"

class ConfigParserTest : public ::testing::Test {
protected:
    ConfigParser parser;
    ServerConfig config;

    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(ConfigParserTest, ParseValidHost) {
    std::string line = "host 127.0.0.1";
    ASSERT_NO_THROW(parser.parseGlobal(line, config));
    EXPECT_EQ(config.host, "127.0.0.1");
}

TEST_F(ConfigParserTest, ParseInvalidHost) {
    std::string line = "host invalid_host";
    EXPECT_THROW(parser.parseGlobal(line, config), ConfigError);
}

TEST_F(ConfigParserTest, ParseDuplicateHost) {
    std::string line1 = "host 127.0.0.1";
    std::string line2 = "host 192.168.1.1";
    ASSERT_NO_THROW(parser.parseGlobal(line1, config));
    EXPECT_THROW(parser.parseGlobal(line2, config), ConfigError);
}

TEST_F(ConfigParserTest, ParseValidPort) {
    std::string line = "port 8080";
    ASSERT_NO_THROW(parser.parseGlobal(line, config));
    EXPECT_EQ(config.port, static_cast<unsigned int>(8080));
}

TEST_F(ConfigParserTest, ParseInvalidPort) {
    std::string line = "port invalid_port";
    EXPECT_THROW(parser.parseGlobal(line, config), ConfigError);
}

TEST_F(ConfigParserTest, ParseDuplicatePort) {
    std::string line1 = "port 8080";
    std::string line2 = "port 8081";
    ASSERT_NO_THROW(parser.parseGlobal(line1, config));
    EXPECT_THROW(parser.parseGlobal(line2, config), ConfigError);
}

TEST_F(ConfigParserTest, ParseValidServerName) {
    std::string line = "server_name localhost";
    ASSERT_NO_THROW(parser.parseGlobal(line, config));
    EXPECT_EQ(config.serverName, "localhost");
}

TEST_F(ConfigParserTest, ParseValidErrorPage) {
    std::string line = "error_page 404 config/default/404.html";
    ASSERT_NO_THROW(parser.parseGlobal(line, config));
    EXPECT_EQ(config.errorPages[404], "config/default/404.html");
}

TEST_F(ConfigParserTest, ParseInvalidErrorPage) {
    std::string line = "error_page 404 config/default//404.html";
    EXPECT_THROW(parser.parseGlobal(line, config), ConfigError);
}

TEST_F(ConfigParserTest, ParseDuplicateErrorPage) {
    std::string line1 = "error_page 404 config/default/404.html";
    std::string line2 = "error_page 404 config/default//404.html";
    ASSERT_NO_THROW(parser.parseGlobal(line1, config));
    EXPECT_THROW(parser.parseGlobal(line2, config), ConfigError);
}

TEST_F(ConfigParserTest, ParseValidClientMaxBodySize) {
    std::string line = "client_max_body_size 1m";
    ASSERT_NO_THROW(parser.parseGlobal(line, config));
    EXPECT_EQ(config.clientMaxBodySize, "1m");
}

TEST_F(ConfigParserTest, ParseInvalidClientMaxBodySize) {
    std::string line = "client_max_body_size invalid_size";
    EXPECT_THROW(parser.parseGlobal(line, config), ConfigError);
}

TEST_F(ConfigParserTest, ParseDuplicateClientMaxBodySize) {
    std::string line1 = "client_max_body_size 1m";
    std::string line2 = "client_max_body_size 2m";
    ASSERT_NO_THROW(parser.parseGlobal(line1, config));
    EXPECT_THROW(parser.parseGlobal(line2, config), ConfigError);
}
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

TEST_F(ConfigParserTest, ParseHost) {
    // Valid cases
    EXPECT_NO_THROW({
        parser.parseGlobal("host 127.0.0.1", config);
        std::cout << "Parsed host: " << config.host << std::endl;
    });

    // Invalid cases
    EXPECT_THROW({
        parser.parseGlobal("host invalid_host", config);
        std::cout << "Parsed host: " << config.host << std::endl;
    }, ConfigError);

    EXPECT_THROW({
        parser.parseGlobal("host ", config);
        std::cout << "Parsed host: " << config.host << std::endl;
    }, ConfigError);

    // Duplicate host
    EXPECT_THROW({
        parser.parseGlobal("host 192.168.1.1", config);
        std::cout << "Parsed host: " << config.host << std::endl;
    }, ConfigError);
}

TEST_F(ConfigParserTest, ParsePort) {
    // Valid cases
    EXPECT_NO_THROW({
        parser.parseGlobal("port 8080", config);
        std::cout << "Parsed port: " << config.port << std::endl;
    });

    // Invalid cases
    EXPECT_THROW({
        parser.parseGlobal("host invalid_port", config);
        std::cout << "Parsed port: " << config.port << std::endl;
    }, ConfigError);

    EXPECT_THROW({
        parser.parseGlobal("port ", config);
        std::cout << "Parsed port: " << config.port << std::endl;
    }, ConfigError);

    // Duplicate host
    EXPECT_THROW({
        parser.parseGlobal("port 8081", config);
        std::cout << "Parsed port: " << config.port << std::endl;
    }, ConfigError);
}

// ServerName is optional so it can be empty
TEST_F(ConfigParserTest, ParseServerName) {
    // Valid cases
    EXPECT_NO_THROW({
        parser.parseGlobal("server_name localhost", config);
        std::cout << "Parsed server name: " << config.serverName << std::endl;
    });

    EXPECT_NO_THROW({
        parser.parseGlobal("", config);
        std::cout << "Parsed server name: " << config.serverName << std::endl;
    });

    // Duplicate server name
    EXPECT_THROW({
        parser.parseGlobal("server_name localhost", config);
        std::cout << "Parsed server name: " << config.serverName << std::endl;
    }, ConfigError);
}

TEST_F(ConfigParserTest, ParseErrorPage) {
    // Valid cases
    EXPECT_NO_THROW({
        parser.parseGlobal("error_page 404 config/default/404.html", config);
        std::cout << "Parsed error page: " << config.errorPages[404] << std::endl;
    });

    // Invalid cases
    EXPECT_THROW({
        parser.parseGlobal("error_page 404 config/default//404.html", config);
        std::cout << "Parsed error page: " << config.errorPages[404] << std::endl;
    }, ConfigError);

    EXPECT_THROW({
        parser.parseGlobal("error_page 404 config/default/505.html", config);
        std::cout << "Parsed error page: " << config.errorPages[404] << std::endl;
    }, ConfigError);

}

TEST_F(ConfigParserTest, ParseValidClientMaxBodySize) {
    // Valid cases
    EXPECT_NO_THROW({
        parser.parseGlobal("client_max_body_size 1M", config);
        std::cout << "Parsed client max body size: " << config.clientMaxBodySize << std::endl;
    });

    // Reset config for the next test
    config.clientMaxBodySize.clear();
    
    EXPECT_NO_THROW({
        parser.parseGlobal("client_max_body_size 1K", config);
        std::cout << "Parsed client max body size: " << config.clientMaxBodySize << std::endl;
    });

    // Reset config for the next test
    config.clientMaxBodySize.clear();

    EXPECT_NO_THROW({
        parser.parseGlobal("client_max_body_size 1G", config);
        std::cout << "Parsed client max body size: " << config.clientMaxBodySize << std::endl;
    });

    // Duplicate client max body size
    EXPECT_THROW({
        parser.parseGlobal("client_max_body_size 1M", config);
        std::cout << "Parsed client max body size: " << config.clientMaxBodySize << std::endl;
    }, ConfigError);

    // Invalid cases
    EXPECT_THROW({
        parser.parseGlobal("client_max_body_size invalid_size", config);
        std::cout << "Parsed client max body size: " << config.clientMaxBodySize << std::endl;
    }, ConfigError);

    EXPECT_THROW({
        parser.parseGlobal("client_max_body_size 1", config);
        std::cout << "Parsed client max body size: " << config.clientMaxBodySize << std::endl;
    }, ConfigError);

    EXPECT_THROW({
        parser.parseGlobal("client_max_body_size 1KB", config);
        std::cout << "Parsed client max body size: " << config.clientMaxBodySize << std::endl;
    }, ConfigError);

    EXPECT_THROW({
        parser.parseGlobal("client_max_body_size 1MB", config);
        std::cout << "Parsed client max body size: " << config.clientMaxBodySize << std::endl;
    }, ConfigError);

    EXPECT_THROW({
        parser.parseGlobal("client_max_body_size 1GB", config);
        std::cout << "Parsed client max body size: " << config.clientMaxBodySize << std::endl;
    }, ConfigError);

    EXPECT_THROW({
        parser.parseGlobal("client_max_body_size 10000KB", config);
        std::cout << "Parsed client max body size: " << config.clientMaxBodySize << std::endl;
    }, ConfigError);
}


#include <gtest/gtest.h>
#include "Config.hpp"
#include "utils/common.hpp"
#include "Error.hpp"
#include <iostream>

class ConfigParserTest : public ::testing::Test {
protected:
    ConfigParser parser;
    ServerConfig server;
    Location location;

    void SetUp() override {
        // Setup code if needed
        location.root = "config/default";
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(ConfigParserTest, ParseHost) {
    // Valid cases
    EXPECT_NO_THROW(parser.parseGlobal("host 127.0.0.1", server));
    EXPECT_EQ(server.host, "127.0.0.1");

    // Invalid cases
    EXPECT_THROW(parser.parseGlobal("host invalid_host", server), ConfigError);
    EXPECT_THROW(parser.parseGlobal("host 1270.0.5.1", server), ConfigError);
    EXPECT_THROW(parser.parseGlobal("host ", server), ConfigError);

    // Duplicate host
    EXPECT_THROW(parser.parseGlobal("host 192.168.1.1", server), ConfigError);
}

TEST_F(ConfigParserTest, ParsePort) {
    // Valid cases
    EXPECT_NO_THROW(parser.parseGlobal("port 8080", server));
    EXPECT_EQ(server.port, 8080);

    // Invalid cases
    EXPECT_THROW(parser.parseGlobal("host invalid_port", server), ConfigError);
    EXPECT_THROW(parser.parseGlobal("port ", server), ConfigError);
    EXPECT_THROW(parser.parseGlobal("port 8081578", server), ConfigError);

    // Duplicate host
    EXPECT_THROW(parser.parseGlobal("port 8081", server), ConfigError);
}

// ServerName is optional so it can be empty
TEST_F(ConfigParserTest, ParseServerName) {
    // Valid cases
    EXPECT_NO_THROW(parser.parseGlobal("server_name localhost", server));
/*         // Optional server_name (empty value)
    EXPECT_NO_THROW(parser.parseGlobal("server_name ", server));
    EXPECT_EQ(server.serverName, ""); */

    // Duplicate server name
    EXPECT_THROW(parser.parseGlobal("server_name localhost", server), ConfigError);
}

TEST_F(ConfigParserTest, ParseErrorPage) {
    // Valid cases
    EXPECT_NO_THROW(parser.parseGlobal("error_page 404 default/404.html", server));

    // Invalid cases
    //EXPECT_THROW(parser.parseGlobal("error_page 404 default//404.html", server), ConfigError);
    //EXPECT_THROW(parser.parseGlobal("error_page 404 default/505.html", server), ConfigError);

}

/* TEST_F(ConfigParserTest, ParseValidClientMaxBodySize) {
    // Valid cases
    EXPECT_NO_THROW(parser.parseGlobal("client_max_body_size 1M", config));
    EXPECT_EQ(config.clientMaxBodySize, "1M");

    // Reset config for the next test
    config.clientMaxBodySize.clear();
    
    EXPECT_NO_THROW(parser.parseGlobal("client_max_body_size 1K", config));
    EXPECT_EQ(config.clientMaxBodySize, "1K");

    // Reset config for the next test
    config.clientMaxBodySize.clear();

    EXPECT_NO_THROW(parser.parseGlobal("client_max_body_size 1G", config));
    EXPECT_EQ(config.clientMaxBodySize, "1G");

    // Duplicate client max body size
    EXPECT_THROW(parser.parseGlobal("client_max_body_size 1M", config), ConfigError);

    // Invalid cases
    EXPECT_THROW(parser.parseGlobal("client_max_body_size invalid_size", config), ConfigError);

    EXPECT_THROW(parser.parseGlobal("client_max_body_size 1", config), ConfigError);

    EXPECT_THROW(parser.parseGlobal("client_max_body_size 1KB", config), ConfigError);

    EXPECT_THROW(parser.parseGlobal("client_max_body_size 1MB", config), ConfigError);

    EXPECT_THROW(parser.parseGlobal("client_max_body_size 1GB", config), ConfigError);

    EXPECT_THROW(parser.parseGlobal("client_max_body_size 10000KB", config), ConfigError);
}
 */
TEST_F(ConfigParserTest, ParseLocationRoot) {
    // Valid cases
    //EXPECT_NO_THROW(parser.parseLocation("root config/default", location)) << "Root path is valid";
    EXPECT_EQ(location.root, "config/default");

    // Invalid cases
    EXPECT_THROW(parser.parseLocation("root ", location), ConfigError) << "Root path is empty";
    EXPECT_THROW(parser.parseLocation("root config//default", location), ConfigError) << "Root path is invalid";

    // Duplicate root
    EXPECT_THROW(parser.parseLocation("root config/default", location), ConfigError) << "Root path is duplicated";
}

TEST_F(ConfigParserTest, ParseLocationIndex) {
    // Valid cases
    EXPECT_NO_THROW(parser.parseLocation("index index.html", location));
    EXPECT_EQ(location.index, "index.html");

    // Invalid cases
    EXPECT_THROW(parser.parseLocation("index ", location), ConfigError);
    EXPECT_THROW(parser.parseLocation("index /index.html", location), ConfigError);
    EXPECT_THROW(parser.parseLocation("index index.html index2.html", location), ConfigError);
    EXPECT_THROW(parser.parseLocation("index x", location), ConfigError);

    // Duplicate index
    EXPECT_THROW(parser.parseLocation("index index.html", location), ConfigError);
}

TEST_F(ConfigParserTest, ParseLocationAutoIndex) {
    // Valid cases
    EXPECT_NO_THROW(parser.parseLocation("autoindex on", location));

    //config.locations[0].isAutoIndex = false; // Reset autoindex for the next test
    
    //EXPECT_NO_THROW(parser.parseLocation("autoindex off", config.locations[0]));

    // Invalid cases
    EXPECT_THROW(parser.parseLocation("autoindex ", location), ConfigError);
    EXPECT_THROW(parser.parseLocation("autoindex invalid", location), ConfigError);
    // Duplicate autoindex
    EXPECT_THROW(parser.parseLocation("autoindex on", location), ConfigError);
}

TEST_F(ConfigParserTest, ParseLocationMethods) {
    // Valid cases
    EXPECT_NO_THROW(parser.parseLocation("methods GET POST", location));

    // Invalid cases
    EXPECT_THROW(parser.parseLocation("methods GET POST PUT", location), ConfigError);

    // Duplicate methods
    EXPECT_THROW(parser.parseLocation("methods GET POST", location), ConfigError);
}

TEST_F(ConfigParserTest, ParseLocationCGIExtension) {
    // Valid cases
    EXPECT_NO_THROW(parser.parseLocation("cgi_extension .php", location));

    // Invalid cases
    EXPECT_THROW(parser.parseLocation("cgi_extension .php", location), ConfigError);

    EXPECT_THROW(parser.parseLocation("cgi_extension .php", location), ConfigError);
}

TEST_F(ConfigParserTest, ParseLocationUploadDir) {
    // Valid cases
    EXPECT_NO_THROW(parser.parseLocation("upload_dir config/http/static", location));

    // Invalid cases
    EXPECT_THROW(parser.parseLocation("upload_dir //config/http/static", location), ConfigError);

    EXPECT_THROW(parser.parseLocation("upload_dir config/http/static", location), ConfigError);
}

TEST_F(ConfigParserTest, ParseLocationReturn) {
    // Valid cases
    EXPECT_NO_THROW(parser.parseLocation("return 301 http://example.com", location));

    // Invalid cases
    EXPECT_THROW(parser.parseLocation("return x x/index.html", location), ConfigError);
    EXPECT_THROW(parser.parseLocation("return ", location), ConfigError);
    EXPECT_THROW(parser.parseLocation("return 301 http://example.com", location), ConfigError);
}

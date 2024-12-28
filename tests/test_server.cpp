#include <gtest/gtest.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../include/Server.hpp"  // Adjust the path as necessary

class ServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(ServerTest, CreateAndBindSocket) {
    ServerConfig config;
    config.port = 8080;
    config.host = "127.0.0.1";

    int sockfd = -1;
    ASSERT_NO_THROW({
        sockfd = createAndBindSocket(config);
    });

    ASSERT_NE(sockfd, -1);

    // Check if the socket is bound correctly
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    ASSERT_EQ(getsockname(sockfd, (struct sockaddr*)&addr, &addr_len), 0);
    ASSERT_EQ(addr.sin_family, AF_INET);
    ASSERT_EQ(ntohs(addr.sin_port), config.port);

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip_str, sizeof(ip_str));
    ASSERT_STREQ(ip_str, config.host.c_str());

    // Close the socket
    close(sockfd);
}


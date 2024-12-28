#include "CgiHandler.hpp"
#include "Constant.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>

HttpResponse CgiHandler::executeCgi(const HttpRequest& /*request*/) {
    HttpResponse response;
    response.version = "HTTP/1.1";

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        response.statusCode = HTTP_INTERNAL_SERVER_ERROR;
        response.statusMessage = HTTP_INTERNAL_SERVER_ERROR_MSG;
        response.body = "Failed to create pipe";
        response.headers["Content-Length"] = std::to_string(response.body.size());
        return response;
    }

    pid_t pid = fork();
    if (pid == -1) {
        response.statusCode = HTTP_INTERNAL_SERVER_ERROR;
        response.statusMessage = HTTP_INTERNAL_SERVER_ERROR_MSG;
        response.body = "Failed to fork process";
        response.headers["Content-Length"] = std::to_string(response.body.size());
        return response;
    }

    if (pid == 0) {
        // Child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        execl("/usr/bin/env", "env", "perl", "/Users/lumik/Webserv/cgi-bin/script.cgi", nullptr);
        _exit(1);
    } else {
        // Parent process
        close(pipefd[1]);
        waitpid(pid, nullptr, 0);

        std::stringstream buffer;
        FILE* file = fdopen(pipefd[0], "r");
        if (file) {
            char c;
            while ((c = fgetc(file)) != EOF) {
                buffer << c;
            }
            fclose(file);
        }
        close(pipefd[0]);

        response.statusCode = HTTP_OK;
        response.statusMessage = HTTP_OK_MSG;
        response.body = buffer.str();
        response.headers["Content-Type"] = "text/html";
        response.headers["Content-Length"] = std::to_string(response.body.size());
    }

    return response;
}
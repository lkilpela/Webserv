#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <fstream>
#include <map>

std::string executeCGIScript(const std::string &scriptPath, const std::map<std::string, std::string> &envVars) {
    int pipefd[2]; // [0] for reading, [1] for writing
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return "500 Internal Server Error";
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return "500 Internal Server Error";
    }

    if (pid == 0) {
        // Child process: Set up environment and execute script
        close(pipefd[0]); // Close read end in child process

        // Redirect stdout to the pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        // Prepare arguments for execve
        std::vector<char*> args;
        args.push_back(const_cast<char*>("/usr/bin/python3")); // Path to Python interpreter
        args.push_back(const_cast<char*>(scriptPath.c_str())); // Path to the script
        args.push_back(nullptr); // Null-terminated array

        // Prepare environment variables for execve
        std::vector<char*> envp;
        for (const auto &envVar : envVars) {
            std::string env = envVar.first + "=" + envVar.second;
            envp.push_back(const_cast<char*>(env.c_str()));
        }
        envp.push_back(nullptr); // Null-terminated array

        // Execute the script
        execve("/usr/bin/python3", args.data(), envp.data());

        // If execve returns, there was an error
        perror("execve");
        exit(1);
    } else {
/*         // Parent process: Read from the pipe
        close(pipefd[1]); // Close write end in parent process

        char buffer[4096];
        std::string output;
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, bytesRead);
        }

        close(pipefd[0]); // Close read end in parent process

        // Wait for child process to finish
        int status;
        waitpid(pid, &status, 0);

        return output;
    } */
}
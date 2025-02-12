#include "Server.hpp"
#include "http/index.hpp"
#include <unistd.h>
#include "utils/Payload.hpp"
#include "Config.hpp"
#include <filesystem>

using http::StatusCode;
using http::Request;
using http::Response;
using std::string;
namespace fs = std::filesystem;

void Server::handleCGI(const Request& request,
						Response& response,
						const string& requestPath, 
						const Location& loc) {
	std::cout << YELLOW "Handling CGI request" RESET << std::endl;
	(void)request;
	std::string scriptPath = loc.root / requestPath.substr(loc.path.size());

	// Validate CGI script
	if (!fs::exists(scriptPath) || !fs::is_regular_file(scriptPath)) {
		response.setFile(StatusCode::NOT_FOUND_404, loc.root / "404.html");
		return;
	}

	// Create a pipe for CGI output
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		response.setFile(StatusCode::INTERNAL_SERVER_ERROR_500, loc.root / "500.html");
		return;
	}

	pid_t pid = fork();
	if (pid == -1) {
		response.setFile(StatusCode::INTERNAL_SERVER_ERROR_500, loc.root / "500.html");
		return;
	}

	if (pid == 0) {  // Child process (Executes CGI script)
		close(pipefd[0]);  // Close unused read end
		dup2(pipefd[1], STDOUT_FILENO);  // Redirect STDOUT to pipe
		close(pipefd[1]);

		char* args[] = {const_cast<char*>(scriptPath.c_str()), NULL};
		char* envp[] = {NULL};  // Environment variables (to be passed to CGI script)
		execve(scriptPath.c_str(), args, envp);

		// If execve fails
		exit(1);
	} else {  // Parent process
		close(pipefd[1]);  // Close unused write end
		char buffer[1024];
		std::string cgiOutput;
		ssize_t bytesRead;

		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytesRead] = '\0';
			cgiOutput += buffer;
		}
		close(pipefd[0]);

		// Set the response body from CGI output
		response.setText(StatusCode::OK_200, cgiOutput);
	}
}

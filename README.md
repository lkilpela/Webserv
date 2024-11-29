# Webserv
Authors : [Lumi K.](https://github.com/lkilpela) / [Michale N.](https://github.com/michale1nguyen1) / [Hoang T.](https://github.com/hoangfin)

- Learning Resources: [Doc](doc/doc.md)

## Main Roles and Responsibilities
1. **Configuration and Routing**

- Handles configuration files and routing.
- Reads, interprets, and applies server settings.

2. **Socket and networking**

- Manages networking, sockets, and connections.
- Ensures the server handles multiple clients and keeps connections stable.

3. **HTTP Processing**

- Handles HTTP requests/responses, CGI.
- Ensures the server follows HTTP standards and is extendable.

## Weekly Goals
### `Week 1`:
#### Goals
- Understand the requirements and foundational concepts.
- Design the basic architecture of the server.
- Set up the development environment.
#### Tasks
1. `Research (All Members)`
- Read `RFC 2616 (HTTP/1.1)` to understand HTTP basics and headers.
- Study non-blocking I/O concepts (`poll, epoll, kqueue`).
- Learn from NGINX configuration files.
- Test requests using telnet and basic HTTP GET/POST using curl.
2. `Setup (Member 1)`
- Create a Makefile for the project.
- Organize directory structure: `/src, /include, /config, /tests`.
3. `Design the Architecture (All Members)`
- Draft a flowchart for handling requests and responses.
- Define classes:
    - `Server` for listening to ports and managing configurations.
    - `Connection` for managing client interactions.
    - `Request` and `Response` for parsing and formatting HTTP data.
4. `Configuration File Design (Member 2)`
- Draft the structure for the configuration file, inspired by NGINX.
5. `Non-blocking I/O Study (Member 3)`
- Prototype a basic poll-based loop to handle multiple connections.

### `Week 2`:
#### Goals
- Implement the core server loop.
- Parse and load configuration files.
- Handle basic HTTP requests.

#### Tasks
1. `Server Setup (Member 1)`
- Implement the Server class:
    - Initialize sockets and bind to ports.
    - Configure non-blocking mode.
    - Handle multiple clients with poll.
2. `Configuration Parsing (Member 2)`
- Implement the configuration file parser:
- Read port, host, and server settings.
- Load routes, error pages, and body size limits.
3. `Basic Request Handling (Member 3)`
- Implement the Request class:
- Parse HTTP request headers.
- Handle `GET` and `POST` methods.
4. `Testing and Debugging (All Members)`
- Use `telnet` to send simple requests and verify responses.
- Write basic tests to validate server functionality.

### `Week 3`:
#### Goals
- Add POST and DELETE support.
- Implement error handling and default pages.
- Support file uploads.

#### Tasks
1. `Advanced Request Handling (Member 1)`
- Implement POST and DELETE methods.
- Handle chunked data in requests.
2. `Error Handling (Member 2)`
- Implement a mechanism for generating HTTP status codes and error pages.
2. `File Uploads (Member 3)`
- Implement logic to accept and save uploaded files to a specified directory.

### `Week 4`:
#### Goals
- Add CGI execution.
- Implement directory listing and redirection.
- Prepare documentation and test cases.

#### Tasks
1. `CGI Support (Member 1)`
- Implement logic for executing CGI scripts (e.g., PHP, Python).
2. `Directory Listing and Redirection (Member 2)`
- Handle directory listing toggles.
- Implement HTTP redirection rules.
2. `Comprehensive Testing (Member 3)`
- Test edge cases, such as invalid requests, large files, and timeout scenarios.
3. `Documentation (All Members)`
- Write a user guide and README.
- Document the configuration file format and examples.


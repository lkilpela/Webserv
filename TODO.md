# TODO

## HTTP

1. Valid Request


## Configuration File

- [ ] Choose the port and host of each server.
- [ ] Setup the server names.
- [ ] Ensure the first server for a host:port is the default for that host:port.
- [ ] Setup default error pages.
- [ ] Limit client body size.
- [ ] Setup routes with the following configurations:
  - [ ] Define accepted HTTP methods for the route.
  - [ ] Define HTTP redirection.
  - [ ] Define a directory or file for file searches.
  - [ ] Turn on or off directory listing.
  - [ ] Set a default file for directory requests.
  - [ ] Execute CGI based on file extension.
  - [ ] Support POST and GET methods.
  - [ ] Accept uploaded files and configure their save location.
  - [ ] Handle CGI specifics:
    - [ ] Use full path as PATH_INFO.
    - [ ] Unchunk chunked requests.
    - [ ] Handle CGI output without content_length using EOF.
    - [ ] Call CGI with the requested file as the first argument.
    - [ ] Run CGI in the correct directory for relative paths.
    - [ ] Ensure server works with one CGI (e.g., php-CGI, Python).

## Router

### GET Method

- [ ] Handle GET request.
- [ ] Handle static file request.
- [ ] Handle directory listing.
- [ ] Handle default file for directory.
- [ ] Handle CGI execution.
- [ ] Handle CGI output.
- [ ] Handle CGI error.
- [ ] Handle CGI not found.
- [ ] Handle CGI not executable.
- [ ] Handle CGI not allowed.
- [ ] Handle CGI not supported.

### POST Method

- [ ] Handle POST request.
- [ ] Handle POST request with file upload.
- [ ] Handle POST request with file upload and CGI execution.

### DELETE Method

- [ ] Handle DELETE request.
- [ ] Handle DELETE request with file deletion.
- [ ] Handle DELETE request with directory deletion.

### Additional Features

- [ ] Support for Query Parameters.
- [ ] Support Range Requests for large file downloads.

## ROUTER

1. GET method
- [ ] Handle GET request
- [ ] Handle static file request
- [ ] Handle directory listing
- [ ] Handle default file for directory
- [ ] Handle CGI execution
- [ ] Handle CGI output
- [ ] Handle CGI error
- [ ] Handle CGI not found
- [ ] Handle CGI not executable
- [ ] Handle CGI not allowed
- [ ] Handle CGI not supported

2. POST method
- [ ] Handle POST request
- [ ] Handle POST request with file upload
- [ ] Handle POST request with file upload and CGI execution

3. DELETE method
- [ ] Handle DELETE request
- [ ] Handle DELETE request with file deletion
- [ ] Handle DELETE request with directory deletion

4. Scenarios for Redirects
Scenario	                        Redirect              Type	Implementation
Directory without trailing slash	301 Moved Permanently	Add trailing slash redirection logic.
Location block with return directive	301 or 302	Add redirectUrl to Location and handle it in Router::handle.
Enforce case sensitivity	301 Moved Permanently	Canonicalize request paths and compare with configured paths.
Force HTTPS	301 Moved Permanently	Redirect HTTP requests to HTTPS.


TBC
- Support for Query Parameters: https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/GET
Why? Ensure paths with query string (/static/file?name=value) are handled correctly. Query string shoul dnot interfere with path matching or file lookup.
- Range Requests for large file downlaod like video streaming -> https://developer.mozilla.org/en-US/docs/Web/HTTP/Range_requests
Why? This ensure efficient resource usage when clients request partial file content, reducing the amount of data transferred and improving download speed.
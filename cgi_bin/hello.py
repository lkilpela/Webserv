#!/usr/bin/env python3
import os
import sys
import urllib.parse
import cgi
import cgitb

cgitb.enable()  # Enable CGI error reporting

UPLOAD_DIR = "/var/www/uploads"  # Adjust path as needed

def serve_file(filename):
    """Serve a requested file via GET."""
    filepath = os.path.join(UPLOAD_DIR, filename)
    
    if not os.path.exists(filepath):
        print("HTTP/1.1 404 Not Found")
        print("Content-Type: text/html")
        print("Connection: keep-alive")
        print()
        print("<html><body><h1>404 Not Found</h1><p>File not found.</p></body></html>")
        return
    
    with open(filepath, "rb") as f:
        file_data = f.read()
    
    print("HTTP/1.1 200 OK")
    print("Content-Type: application/octet-stream")  # Change if needed
    print(f"Content-Length: {len(file_data)}")
    print("Connection: keep-alive")
    print()
    sys.stdout.buffer.write(file_data)  # Write binary response

def handle_file_upload():
    """Handle a file upload via POST."""
    form = cgi.FieldStorage()
    
    if "file" not in form or not form["file"].filename:
        print("HTTP/1.1 400 Bad Request")
        print("Content-Type: text/html")
        print("Connection: keep-alive")
        print()
        print("<html><body><h1>400 Bad Request</h1><p>No file uploaded.</p></body></html>")
        return
    
    file_item = form["file"]
    filename = os.path.basename(file_item.filename)
    filepath = os.path.join(UPLOAD_DIR, filename)

    with open(filepath, "wb") as f:
        f.write(file_item.file.read())

    print("HTTP/1.1 200 OK")
    print("Content-Type: text/html")
    print("Connection: keep-alive")
    print()
    print(f"<html><body><h1>File Uploaded</h1><p>File saved as: {filename}</p></body></html>")

def main():
    """Process GET and POST requests."""
    request_method = os.environ.get("REQUEST_METHOD", "GET")

    if request_method == "GET":
        query_string = os.environ.get("QUERY_STRING", "")
        params = urllib.parse.parse_qs(query_string)
        filename = params.get("file", [None])[0]

        if filename:
            serve_file(filename)
        else:
            print("HTTP/1.1 400 Bad Request")
            print("Content-Type: text/html")
            print("Connection: keep-alive")
            print()
            print("<html><body><h1>400 Bad Request</h1><p>Missing file parameter.</p></body></html>")
    
    elif request_method == "POST":
        handle_file_upload()
    
    else:
        print("HTTP/1.1 405 Method Not Allowed")
        print("Content-Type: text/html")
        print("Connection: keep-alive")
        print()
        print("<html><body><h1>405 Method Not Allowed</h1></body></html>")

if __name__ == "__main__":
    main()



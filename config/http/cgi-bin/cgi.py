#!/usr/bin/env python3

import cgi
import cgitb

cgitb.enable()  # Enable CGI error reporting

print("Content-Type: text/html")  # HTML is following
print()  # Blank line, end of headers

print("<html><head>")
print("<title>CGI Script Output</title>")
print("</head><body>")
print("<h1>Hello, World!</h1>")
print("</body></html>")
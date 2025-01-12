#!/usr/bin/env python3

import os
import cgi

def main():
    # Get the PATH_INFO environment variable
    path_info = os.environ.get('PATH_INFO', '')

    # Print the HTTP headers
    print("Content-Type: text/plain")
    print()

    # Print the path info
    print(f"Path Info: {path_info}")

    # Print additional information
    print("Hello, this is a CGI script!")
    print("Here are the environment variables:")
    for key, value in os.environ.items():
        print(f"{key}: {value}")

if __name__ == "__main__":
    main()
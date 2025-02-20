#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace http {
	/**
	 * Example of a full uri
	 * `https://hoatran:WDAcasdYYT@www.example.com:8080/path/to/resource?search=apple&limit=10#section2`
	 *
	 * - scheme = `https`
	 * - user = `hoatran`
	 * - password = `WDAcasdYYT`
	 * - host = `www.example.com`
	 * - port = `8080`
	 * - path = `/path/to/resource`
	 * - query = `search=apple&limit=10`
	 * - fragment = `section2`
	 *
	*/
	struct Url {
		std::string scheme;
        std::string user;
        std::string password;
        std::string host;
        std::string port;
        std::string path;
        std::string query;
        std::string fragment;

		Url() = default;
		Url(const Url&) = default;
		~Url() = default;
		Url& operator=(const Url&) = default;
	};

	struct MultipartElement {
		std::string name;
		std::string fileName;
		std::string contentType;
		std::vector<uint8_t> rawData;
	};
}
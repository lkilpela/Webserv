#pragma once

#include <string>

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
	struct Url final {
		std::string scheme;
        std::string user;
        std::string password;
        std::string host;
        std::string port;
        std::string path;
        std::string query;
        std::string fragment;
	};

	template <typename Iterator>
	Url parseUri(const Iterator begin, const Iterator end) {
		Uri result;

		// RFC 3986, 3.1. Scheme
		auto i = begin;
		if (i == end || !isAlphaChar(*begin))
			throw RequestError{"Invalid scheme"};

		result.scheme.push_back(*i++);

		for (; i != end && (isAlphaChar(*i) || isDigitChar(*i) || *i == '+' || *i == '-' || *i == '.'); ++i)
			result.scheme.push_back(*i);

		if (i == end || *i++ != ':')
			throw RequestError{"Invalid scheme"};
		if (i == end || *i++ != '/')
			throw RequestError{"Invalid scheme"};
		if (i == end || *i++ != '/')
			throw RequestError{"Invalid scheme"};

		// RFC 3986, 3.2. Authority
		std::string authority = std::string(i, end);

		// RFC 3986, 3.5. Fragment
		const auto fragmentPosition = authority.find('#');
		if (fragmentPosition != std::string::npos)
		{
			result.fragment = authority.substr(fragmentPosition + 1);
			authority.resize(fragmentPosition); // remove the fragment part
		}

		// RFC 3986, 3.4. Query
		const auto queryPosition = authority.find('?');
		if (queryPosition != std::string::npos)
		{
			result.query = authority.substr(queryPosition + 1);
			authority.resize(queryPosition); // remove the query part
		}

		// RFC 3986, 3.3. Path
		const auto pathPosition = authority.find('/');
		if (pathPosition != std::string::npos)
		{
			// RFC 3986, 3.3. Path
			result.path = authority.substr(pathPosition);
			authority.resize(pathPosition);
		}
		else
			result.path = "/";

		// RFC 3986, 3.2.1. User Information
		std::string userinfo;
		const auto hostPosition = authority.find('@');
		if (hostPosition != std::string::npos)
		{
			userinfo = authority.substr(0, hostPosition);

			const auto passwordPosition = userinfo.find(':');
			if (passwordPosition != std::string::npos)
			{
				result.user = userinfo.substr(0, passwordPosition);
				result.password = userinfo.substr(passwordPosition + 1);
			}
			else
				result.user = userinfo;

			result.host = authority.substr(hostPosition + 1);
		}
		else
			result.host = authority;

		// RFC 3986, 3.2.2. Host
		const auto portPosition = result.host.find(':');
		if (portPosition != std::string::npos)
		{
			// RFC 3986, 3.2.3. Port
			result.port = result.host.substr(portPosition + 1);
			result.host.resize(portPosition);
		}

		return result;
	}
}

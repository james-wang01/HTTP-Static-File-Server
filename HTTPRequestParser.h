// An http request parser.
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class HTTPRequest;

// The parsed representation of the entire config.
class HTTPRequest {
public:
    std::string ToString();
    std::unordered_map<std::string, std::string> ToMap();
    std::vector<std::string> statements_;
};

// The driver that parses a config file and generates an NginxConfig.
class HTTPRequestParser {
public:
    HTTPRequestParser() {}

    // read from socket and parse request
    bool Parse(int sockfd, HTTPRequest *request);

private:
    enum TokenType {
        TOKEN_TYPE_KEY = 0,
        TOKEN_TYPE_CRLF = 1,
        TOKEN_TYPE_SP = 2,
        TOKEN_TYPE_EOF = 3,
        TOKEN_TYPE_ERROR = 4
    };

    enum ParserState {
        STATE_REQUEST_METHOD = 0,
        STATE_REQUEST_URL = 1,
        STATE_REQUEST_HTTP_VERSION = 2,
        STATE_HEADER = 3,
        STATE_HEADER_VALUE = 4
    };

    TokenType ParseToken(int sockfd, std::string *value);
};

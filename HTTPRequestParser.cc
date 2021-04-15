#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <unordered_map>

#include "HTTPRequestParser.h"

using namespace std;

void error() {
    printf("Parse Error");
    exit(1);
}

string HTTPRequest::ToString() {
    string ret;
    for (int i = 0; i < statements_.size(); i++) {
        ret.append(statements_[i]);
        ret.append("\r\n");
    }
    return ret;
}

unordered_map<string, string> HTTPRequest::ToMap() {
    unordered_map<string, string> ret;
    ret["METHOD"] = statements_[0];
    ret["URL"] = statements_[1];
    ret["VERSION"] = statements_[2];

    for (int i = 3; i < statements_.size();i += 2) {
        pair<string, string> p (statements_[i], statements_[i+1]);
        ret.insert(p);
    }
    return ret;
}

bool HTTPRequestParser::Parse(int sockfd, HTTPRequest* request) {
    ParserState state = STATE_REQUEST_METHOD;
    while (true) {
        string statement;
        TokenType statementType = ParseToken(sockfd, &statement);
        if (statementType == TOKEN_TYPE_ERROR) {
            error();
        }
        switch (state) {
            case STATE_REQUEST_METHOD:
                if (statementType != TOKEN_TYPE_SP) {
                    error();
                } else {
                    request->statements_.push_back(statement);
                    state = STATE_REQUEST_URL;
                }
                continue;
            case STATE_REQUEST_URL:
                if (statementType != TOKEN_TYPE_SP) {
                    error();
                } else {
                    request->statements_.push_back(statement);
                    state = STATE_REQUEST_HTTP_VERSION;
                }
                continue;
            case STATE_REQUEST_HTTP_VERSION:
                if (statementType == TOKEN_TYPE_SP) {
                    // still part of request url
                    request->statements_.back().append(" " + statement);
                } else if (statementType != TOKEN_TYPE_CRLF) {
                    error();
                } else {
                    request->statements_.push_back(statement);
                    state = STATE_HEADER;
                }
                continue;
            case STATE_HEADER:
                switch (statementType) {
                    case TOKEN_TYPE_CRLF:
                        // done parsing header
                        return true;
                    case TOKEN_TYPE_KEY:
                        request->statements_.push_back(statement);
                        request->statements_.push_back(""); // for value
                        state = STATE_HEADER_VALUE;
                        char c;
                        read(sockfd, &c, 1); // eat space
                        continue;
                    default:
                        error();
                }
            case STATE_HEADER_VALUE:
                switch (statementType) {
                    case TOKEN_TYPE_CRLF:
                        request->statements_.back().append(statement);
                        state = STATE_HEADER;
                        continue;
                    case TOKEN_TYPE_KEY:
                        request->statements_.back().append(statement + ":");
                        continue;
                    case TOKEN_TYPE_SP:
                        request->statements_.back().append(statement + " ");
                        continue;
                    default:
                        error();
                }
        }
    }
}

HTTPRequestParser::TokenType HTTPRequestParser::ParseToken(int sockfd, string *value) {
    char buf;
    while (true) {
        int n = read(sockfd, &buf, 1);
        printf("%c", buf);
        if (n == -1) {
        } else if (n == 0) {
            return TOKEN_TYPE_EOF;
        } else if (n == 1) {
            switch (buf) {
                case ' ':
                    return TOKEN_TYPE_SP;
                case '\n':
                    return TOKEN_TYPE_CRLF;
                case ':':
                    return TOKEN_TYPE_KEY;
                default:
                    *value += buf;
                    continue;
            }
        } else {
            return TOKEN_TYPE_ERROR;
        }
    }
}
#ifndef VALUES
#define VALUES
#define SERVER_IP           (char*)"34.254.242.81"
#define SERVER_PORT         8080

#define APP_JSON            "application/json"
#define APP_FORM            "application/x-www-form-urlencoded"
#define APP_OCTET           "application/octet-stream"

#define GET                 "GET"
#define POST                "POST"

#define HTTP_OK             200
#define HTTP_404            404

#define PATH_REG            "/api/v1/tema/auth/register"
#define PATH_LOGIN          "/api/v1/tema/auth/login"
#define PATH_LIB_ACC        "/api/v1/tema/library/access"
#define PATH_LIB_BOOKS      "/api/v1/tema/library/books"
#define PATH_LOGOUT         "/api/v1/tema/auth/logout"

#define ERROR_PTR           (void*)1

#endif

#include "compatibility.hpp"

typedef enum _METHOD {
    get, post
} METHOD;

class IJSONify {
public:
    /* creates a string from a vector of pairs, used for inlining a stored json */
    virtual std::string to_json(std::vector<std::pair<std::string, std::string>> delta) = 0;
    /* reverses the above function, creates a vector of pairs starting from an inlined json */
    virtual std::vector<std::pair<std::string, std::string>> from_json(std::string json) = 0;
};

class JSONify : public IJSONify {
public:
    std::string to_json(std::vector<std::pair<std::string, std::string>> delta) {
        std::string json = "{";
        for (auto it = delta.begin(); it != delta.end(); it++) {
            json += "\"" + it->first + "\":\"" + it->second + "\"";
            if (it != delta.end() - 1) json += ",";
        }
        json += "}";
        return json;
    }

    std::vector<std::pair<std::string, std::string>> from_json(std::string json) {
        std::vector<std::pair<std::string, std::string>> delta;
        std::string key, value;
        bool is_key = true;
        for (auto it = json.begin(); it != json.end(); it++) {
            if (*it == '"' or *it == '{' or *it == '}') continue;
            if (*it == ':') {
                is_key = false;
                continue;
            }
            if (*it == ',') {
                is_key = true;
                delta.push_back(std::make_pair(key, value));
                key = value = "";
                continue;
            }
            if (is_key) key += *it;
            else value += *it;
        }
        delta.push_back(std::make_pair(key, value));
        return delta;
    }
};


class Request {
private:
    METHOD method;
    std::string url;
    std::vector<std::pair<std::string, std::string>> cookies;
    std::pair<std::string, std::string> *JWT = nullptr;
    Request() {}

public:
    Request(METHOD method, std::string url) {
        this->method = method;
        this->url = url;
    }

    METHOD get_method() {
        return this->method;
    }

    std::string get_url() {
        return this->url;
    }

    std::vector<std::pair<std::string, std::string>> get_cookies() {
        return this->cookies;
    }

    std::pair<std::string, std::string> *get_JWT() {
        return this->JWT;
    }

    void set_new_cookie(std::string key, std::string value) {
        this->cookies.push_back({key, value});
    }

    void set_JWT(std::string key, std::string value) {
        this->JWT = new std::pair<std::string, std::string>(key, value);
    }

    char** bonk_cookies() {
        char **bufcook = (char**)malloc(this->get_cookies().size() * 2 * sizeof(char*));
        for (int i = 0; i < this->get_cookies().size(); i++) {
            bufcook[i * 2] = (char*)malloc(this->get_cookies()[i].first.size() * sizeof(char));
            bufcook[i * 2 + 1] = (char*)malloc(this->get_cookies()[i].second.size() * sizeof(char));
            strcpy(bufcook[i * 2], this->get_cookies()[i].first.c_str());
            strcpy(bufcook[i * 2 + 1], this->get_cookies()[i].second.c_str());
        }
        return bufcook;
    }

    virtual char* compose_message() = 0;
    void (*send_message)(int, char*) = [](int sockfd, char* message) {
        send_to_server(sockfd, message);
    };
};

class GETRequest : public Request {
    std::vector<std::string> params;
public:
    GETRequest(std::string url, std::vector<std::string> params) : Request(get, url) {
        this->params = params;
    }
    GETRequest(std::string url) : Request(get, url) {}

    char* query_gen();
    char* compose_message();
};

class POSTRequest : public Request {
private:
    const IJSONify* jsonifier = new JSONify();
    std::vector<std::pair<std::string, std::string>> *body;
public:
    POSTRequest(std::string url) : Request(post, url) {
        this->body = new std::vector<std::pair<std::string, std::string>>();
    }

    /* get for body */
    std::vector<std::pair<std::string, std::string>>* get_body() {
        return this->body;
    }

    char* compose_message();
};

class Response {
private:
    std::string message;
    int status_code;
    std::vector<std::pair<std::string, std::string>> cookies;
    std::string body; // will probably be in json format
    const IJSONify* jsonifier = new JSONify();
    Response() {}

public:
    Response(std::string message, std::string code, std::string body) {
        this->message = message;
        this->status_code = std::stoi(code);
        this->body = body;
    }

    std::string get_message() {
        return this->message;
    }

    int get_status_code() {
        return this->status_code;
    }

    std::vector<std::pair<std::string, std::string>> get_cookies() {
        return this->cookies;
    }

    std::string get_body() {
        return this->body;
    }

    void set_cookies(std::vector<std::pair<std::string, std::string>> cookies) {
        this->cookies = cookies;
    }

    void parse_message();
    void print_response();

    /* only use for application/json type bodies */
    std::vector<std::pair<std::string, std::string>> get_json_body() {
        return ((JSONify*)this->jsonifier)->from_json(this->body);
    }
};

Request* parse_stdin(std::string command);
Response* parse_response(char* response);
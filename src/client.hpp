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
            if (*it == '"') continue;
            if (*it == ':') {
                is_key = false;
                continue;
            }
            if (*it == ',') {
                delta.push_back({key, value});
                key = "";
                value = "";
                is_key = true;
                continue;
            }
            if (is_key) key += *it;
            else value += *it;
        }
        delta.push_back({key, value});
        return delta;
    }
};


class Request {
private:
    METHOD method;
    std::string url;
    std::vector<std::pair<std::string, std::string>> cookies;
    Request() {}

public:
    Request(METHOD method, std::string url, std::vector<std::pair<std::string, std::string>> cookies) {
        this->method = method;
        this->url = url;
        this->cookies = cookies;
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
public:
    GETRequest(std::string url, std::vector<std::pair<std::string, std::string>> cookies) : Request(get, url, cookies) {}
    char* compose_message();
};

class POSTRequest : public Request {
private:
    const IJSONify* jsonifier = new JSONify();
    std::vector<std::pair<std::string, std::string>> body;
public:
    POSTRequest(std::string url, std::vector<std::pair<std::string, std::string>> cookies) : Request(post, url, cookies) {}

    /* get for body */
    std::vector<std::pair<std::string, std::string>> get_body() {
        return this->body;
    }

    char* compose_message();
};

class Response {
private:
    std::string message;
    std::string status_code;
    std::string status_message;
    std::map<std::string, std::string> headers;
    std::string body;
    IJSONify* jsonifier = new JSONify();
    Response() {}

public:
    Response(std::string message) {
        this->message = message;
        this->status_code = "";
        this->status_message = "";
        this->headers = {};
        this->body = "";
    }

    std::string get_status_code() {
        return this->status_code;
    }

    std::string get_status_message() {
        return this->status_message;
    }

    std::map<std::string, std::string> get_headers() {
        return this->headers;
    }

    std::string get_body() {
        return this->body;
    }

    void parse_message();
    void print_response();
};

Request* parse_stdin(std::string command);
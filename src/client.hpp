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

#endif

class Request {
private:
    std::string method;
    std::string url;
    std::string body;
    std::map<std::string, std::string> headers;

public:
    Request(std::string method, std::string url, std::string body, std::map<std::string, std::string> headers) {
        this->method = method;
        this->url = url;
        this->body = body;
        this->headers = headers;
    }

    std::string get_method() {
        return this->method;
    }

    std::string get_url() {
        return this->url;
    }

    std::string get_body() {
        return this->body;
    }

    std::map<std::string, std::string> get_headers() {
        return this->headers;
    }

    char* compose_message();
    void send_message(int server_socket);
};

class Response {
private:
    std::string message;
    std::string status_code;
    std::string status_message;
    std::map<std::string, std::string> headers;
    std::string body;

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

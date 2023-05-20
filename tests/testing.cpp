#include <gtest/gtest.h>
#include <sstream>
#include <fstream>
#include <arpa/inet.h>
#include <fcntl.h>
#include "../src/client.hpp"


// Test if the client instance exists
TEST(Client, ClientExists) {
    // Create an instance of the Main class
    Main *client = Main::get_instance();

    // Test if the client instance exists
    EXPECT_FALSE(client == nullptr);
}

TEST(Client, ClientConnects) {
    // Create an instance of the Main class
    Main *client = Main::get_instance();

    client->set_sockfd(open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0));
    int optval = 1;
    setsockopt(client->get_sockfd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    EXPECT_TRUE(client->get_sockfd() > 0);
}

static FILE *f;
TEST(Parser, Register) {
    f = freopen("tests/testcases", "r", stdin);

    // Create an instance of the Main class
    Main *client = Main::get_instance();

    std::string command = "register";

    // parse the request
    auto req = parse_stdin(command, client);

    // check if the request is valid
    EXPECT_FALSE(req == nullptr);
    EXPECT_FALSE(req == (Request*)ERROR_PTR);

    delete req;
}

TEST(Parser, RegisterSend) {
    // Create an instance of the Main class
    Main *client = Main::get_instance();

    std::string command = "register";
    rewind(stdin);

    // parse the request
    auto req = parse_stdin(command, client);

    // redo connection
    close(client->get_sockfd());
    client->set_sockfd(open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0));
    int optval = 1;
    setsockopt(client->get_sockfd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // send the request
    char* message = req->compose_message();
    req->send_message(client->get_sockfd(), message);

    // receive the response
    char* response = receive_from_server(client->get_sockfd());
    Response* res = parse_response(response);
    EXPECT_FALSE(res == (Response*)ERROR_PTR);
    EXPECT_FALSE(res == nullptr);

    // check if request returned 201 or 400
    EXPECT_TRUE(res->get_status_code() == 201 or res->get_status_code() == 400);
    EXPECT_TRUE(res->get_message().find("Created") or res->get_message().find("Bad Request"));

    delete req; delete res;
}

TEST(Parser, Login) {
    // Create an instance of the Main class
    Main *client = Main::get_instance();

    std::string command = "login";
    rewind(stdin);

    // parse the request
    auto req = parse_stdin(command, client);

    // check if the request is valid
    EXPECT_FALSE(req == nullptr);
    EXPECT_FALSE(req == (Request*)ERROR_PTR);

    // redo connection
    close(client->get_sockfd());
    client->set_sockfd(open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0));
    int optval = 1;
    setsockopt(client->get_sockfd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // send the request
    char* message = req->compose_message();
    req->send_message(client->get_sockfd(), message);

    // receive the response
    char* response = receive_from_server(client->get_sockfd());
    Response* res = parse_response(response);

    // check if request returned 200 or 400
    EXPECT_TRUE(res->get_status_code() == 200 or res->get_status_code() == 400);
    EXPECT_TRUE(res->get_message().find("OK") or res->get_message().find("Bad Request"));

    // check if the cookies are set
    EXPECT_FALSE(res->get_cookies().empty());

    // expect true (find if there is any cookie in get_cookies that contains connect)
    bool ok = false;
    for (auto cookie : res->get_cookies()) {
        if (cookie.first.find("connect") != std::string::npos) {
            ok = true;
            // set the session cookie in main (for future tests)
            client->set_session_cookie(cookie);
            break;
        }
    }
    EXPECT_TRUE(ok);

    delete req; delete res;
}

TEST(Parser, EnterLibrary) {
    // Create an instance of the Main class
    Main *client = Main::get_instance();

    // hardcoded stdin
    std::string command = "enter_library";

    // parse the request
    auto req = parse_stdin(command, client);
    fclose(stdin);

    // check if the request is valid
    EXPECT_FALSE(req == nullptr);
    EXPECT_FALSE(req == (Request*)ERROR_PTR);

    // redo connection
    close(client->get_sockfd());
    client->set_sockfd(open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0));
    int optval = 1;
    setsockopt(client->get_sockfd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // send the request
    char* message = req->compose_message();
    req->send_message(client->get_sockfd(), message);

    // receive the response
    char* response = receive_from_server(client->get_sockfd());
    Response* res = parse_response(response);

    // check if request returned 200 or 400
    EXPECT_TRUE(res->get_status_code() == 200 or res->get_status_code() == 400);
    EXPECT_TRUE(res->get_message().find("OK") or res->get_message().find("Bad Request"));

    // expect true (find if there is any JWT token in the response)
    bool ok = false;
    std::vector<std::pair<std::string, std::string>> items = res->get_json_body();
    for (auto iter : items) {
        if (iter.first.find("token") != std::string::npos) {
            ok = true;
            // add the JWT token to the main instance
            client->set_JWT_token(iter);
            break;
        }
    }
    EXPECT_TRUE(ok);

    delete req; delete res;
}

TEST(Library, AddBook) {
    f = freopen("tests/testcasesLibrary", "r", stdin);
    // Create an instance of the Main class
    Main *client = Main::get_instance();
    
    std::string command = "add_book";

    // parse the request
    auto req = parse_stdin(command, client);

    // check if the request is valid
    EXPECT_FALSE(req == nullptr);
    EXPECT_FALSE(req == (Request*)ERROR_PTR);

    // redo connection
    close(client->get_sockfd());
    client->set_sockfd(open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0));
    int optval = 1;
    setsockopt(client->get_sockfd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // send the request
    char* message = req->compose_message();
    req->send_message(client->get_sockfd(), message);

    // receive the response
    char* response = receive_from_server(client->get_sockfd());
    Response* res = parse_response(response);

    // check if request returned 200 or 400
    EXPECT_FALSE(res->get_status_code() == 403);
    EXPECT_TRUE(res->get_status_code() == 200 or res->get_status_code() == 400);
    EXPECT_TRUE(res->get_message().find("OK") or res->get_message().find("Bad Request"));

    delete req; delete res;
}

TEST(Library, GetBooks) {
    // Create an instance of the Main class
    Main *client = Main::get_instance();

    std::string command = "get_books";

    // parse the request
    auto req = parse_stdin(command, client);

    // check if the request is valid
    EXPECT_FALSE(req == nullptr);
    EXPECT_FALSE(req == (Request*)ERROR_PTR);

    // redo connection
    close(client->get_sockfd());
    client->set_sockfd(open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0));
    int optval = 1;
    setsockopt(client->get_sockfd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // send the request
    char* message = req->compose_message();
    req->send_message(client->get_sockfd(), message);

    // receive the response
    char* response = receive_from_server(client->get_sockfd());
    Response* res = parse_response(response);

    // check if request returned 200 or 400
    EXPECT_FALSE(res->get_status_code() == 403);
    EXPECT_TRUE(res->get_status_code() == 200 or res->get_status_code() == 400);
    EXPECT_TRUE(res->get_message().find("OK") or res->get_message().find("Bad Request"));

    // expect true (find if there is any book in the response)
    bool ok = false;
    if (res->get_body().size() > 0) {
        ok = true;
    } // ¯\_(ツ)_/¯
    EXPECT_TRUE(ok);

    delete req; delete res;
}

TEST(Library, GetBook) {
    // Create an instance of the Main class
    Main *client = Main::get_instance();

    std::string command = "get_book";

    // parse the request
    auto req = parse_stdin(command, client);

    // check if the request is valid
    EXPECT_FALSE(req == nullptr);
    EXPECT_FALSE(req == (Request*)ERROR_PTR);

    // redo connection
    close(client->get_sockfd());
    client->set_sockfd(open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0));
    int optval = 1;
    setsockopt(client->get_sockfd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // send the request
    char* message = req->compose_message();
    req->send_message(client->get_sockfd(), message);

    // receive the response
    char* response = receive_from_server(client->get_sockfd());
    Response* res = parse_response(response);

    // check if request returned 200 or 400
    EXPECT_FALSE(res->get_status_code() == 403);
    EXPECT_TRUE(res->get_status_code() == 200 or res->get_status_code() == 400);
    EXPECT_TRUE(res->get_message().find("OK") or res->get_message().find("Bad Request"));

    // expect true (find if there is any book in the response)
    bool ok = false;
    if (res->get_body().size() > 0) {
        ok = true;
    } // ¯\_(ツ)_/¯
    EXPECT_TRUE(ok);

    delete req; delete res;
}

TEST(Library, DeleteBook) {
    // Create an instance of the Main class
    Main *client = Main::get_instance();

    std::string command = "delete_book";

    // parse the request
    auto req = parse_stdin(command, client);
    fclose(f);

    // check if the request is valid
    EXPECT_FALSE(req == nullptr);
    EXPECT_FALSE(req == (Request*)ERROR_PTR);

    // redo connection
    close(client->get_sockfd());
    client->set_sockfd(open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0));
    int optval = 1;
    setsockopt(client->get_sockfd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // send the request
    char* message = req->compose_message();
    req->send_message(client->get_sockfd(), message);

    // receive the response
    char* response = receive_from_server(client->get_sockfd());
    Response* res = parse_response(response);

    // check if request returned 200 or 400
    EXPECT_FALSE(res->get_status_code() == 403);
    EXPECT_TRUE(res->get_status_code() == 200 or res->get_status_code() == 404);
    EXPECT_TRUE(res->get_message().find("OK") or res->get_message().find("Not Found"));

    delete req; delete res;
}

TEST(Cleanup, Logout) {
    // Create an instance of the Main class
    Main *client = Main::get_instance();

    std::string command = "logout";

    // parse the request
    auto req = parse_stdin(command, client);

    // check if the request is valid
    EXPECT_FALSE(req == nullptr);
    EXPECT_FALSE(req == (Request*)ERROR_PTR);

    // redo connection
    close(client->get_sockfd());
    client->set_sockfd(open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0));
    int optval = 1;
    setsockopt(client->get_sockfd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // send the request
    char* message = req->compose_message();
    req->send_message(client->get_sockfd(), message);

    // receive the response
    char* response = receive_from_server(client->get_sockfd());
    Response* res = parse_response(response);

    // check if request returned 200 or 400
    EXPECT_TRUE(res->get_status_code() == 200 or res->get_status_code() == 400);
    EXPECT_TRUE(res->get_message().find("OK") or res->get_message().find("Bad Request"));

    EXPECT_TRUE(client->get_session_cookie().first == "" and client->get_session_cookie().second == "");
    EXPECT_TRUE(client->get_JWT_token().first == "" and client->get_JWT_token().second == "");

    delete req; delete res;
}

TEST(DragosteaDinTeiHardstyleGymRemixZyzzcore, CheckIfItBreaks) {
    f = freopen("tests/testcasesDeath", "r", stdin);
    // Create an instance of the Main class
    Main *client = Main::get_instance();

    std::string command = "login";
    auto req = parse_stdin(command, client);

    // redo connection
    close(client->get_sockfd());
    client->set_sockfd(open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0));
    int optval = 1;
    setsockopt(client->get_sockfd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // send the request
    char* message = req->compose_message();
    req->send_message(client->get_sockfd(), message);

    // receive the response
    char* response = receive_from_server(client->get_sockfd());
    Response* res = parse_response(response);

    // check if request returned 200 or 400
    EXPECT_TRUE(res->get_status_code() == 200 or res->get_status_code() == 400);
    EXPECT_TRUE(res->get_message().find("OK") or res->get_message().find("Bad Request"));

    delete req; delete res;

    command = "get_books";
    req = parse_stdin(command, client);

    EXPECT_TRUE(req == (Request*)ERROR_PTR);

    command = "logout";
    req = parse_stdin(command, client);

    // redo connection
    close(client->get_sockfd());
    client->set_sockfd(open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0));
    optval = 1;
    setsockopt(client->get_sockfd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // send the request
    message = req->compose_message();
    req->send_message(client->get_sockfd(), message);

    // receive the response
    response = receive_from_server(client->get_sockfd());
    res = parse_response(response);

    // check if request returned 200 or 400
    EXPECT_TRUE(res->get_status_code() == 200 or res->get_status_code() == 400);
    EXPECT_TRUE(res->get_message().find("OK") or res->get_message().find("Bad Request"));

    command = "enter_library";
    req = parse_stdin(command, client);

    EXPECT_TRUE(req == (Request*)ERROR_PTR);

    command = "exit";
    req = parse_stdin(command, client);
    
    EXPECT_TRUE(req == nullptr);
}

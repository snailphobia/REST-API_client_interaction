#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "client.hpp"

static int server_socket;

/**
 * the unfortunate byproducts of the industrial revolution
 * this function is to be called right before sending a request 
 */
static bool are_you_still_there(int sockfd) {
    /* set the socket as non-blocking, to not stall the program */
    int flags = fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);

    /* check if the socket is still readable */
    char ender[1] = {};
    int bytes = recv(sockfd, ender, 1, MSG_PEEK);
    try {
        if (bytes == -1)
            throw std::runtime_error("Connection timed out\n");
        if (bytes == 0)
            throw std::runtime_error("Socket not usable ??? ???\n");
    } catch (const std::runtime_error &e) {
        std::cerr << e.what();
        return false;
    }
    return true;
}

std::pair<std::string, std::string> session_cookie;
std::pair<std::string, std::string> JWT_token;

int main(void) {

    server_socket = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    std::string command;
    std::cout << "Socket opened: " << server_socket << "\n";
    while(true) {
        if (!are_you_still_there(server_socket)) {
            close_connection(server_socket);
            server_socket = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
        }
        std::cin >> command;
        auto req = parse_stdin(command);

        /* most advanced try-catch block in poli */
        if (req == nullptr) break;
        if (req == (Request*)ERROR_PTR) continue;

        /* send the request to the server */
        char* message = req->compose_message();
        std::cout << message << "\n";
        req->send_message(server_socket, message);

        /* receive the response from the server */
        char* response = receive_from_server(server_socket);
        Response* res = parse_response(response);

        /* check if the response is an error */
        if (res == (Response*)ERROR_PTR) continue;

        /* print response code and message, along with body of the response */
        std::cout << res->get_status_code() << " " << res->get_message() << "\n";
        std::cout << res->get_body() << "\n";

        res->parse_message();
    }
    return 0;
}

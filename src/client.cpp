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
    char ender[1024];
    int bytes = recv(sockfd, ender, 1024, MSG_PEEK);
    try {
        if (bytes == -1)
            throw std::runtime_error("Connection timed out\n");
        if (bytes == 0)
            throw std::runtime_error("Socket not readable ??? ???\n");
    } catch (const std::runtime_error &e) {
        std::cerr << e.what();
        return false;
    }
    return true;
}

int main(void) {

    server_socket = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    std::string command;
    std::cout << "Socket opened: " << server_socket << "\n";
    while(true) {
        std::cin >> command;
        auto req = parse_stdin(command);

        /* most advanced try-catch block in poli */
        if (req == nullptr) break;
        if (req == (Request*)ERROR_PTR) continue;

        /* send the request to the server */
        char* message = req->compose_message();
        std::cout << ((POSTRequest*)req)->get_body()->size() << "\n";
        if (!are_you_still_there(server_socket))
            server_socket = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
        req->send_message(server_socket, message);

        /* receive the response from the server */
        char* response = receive_from_server(server_socket);
        std::cout << response << "\n";
    }
    return 0;
}

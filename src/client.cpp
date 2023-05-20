#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "client.hpp"

/**
 * the unfortunate byproducts of the industrial revolution
 * this function is to be called right before sending a request 
 */
bool Main::are_you_still_there(int sockfd) {
    /* set the socket as non-blocking, to not stall the program */
    int flags = fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);

    /* send a zero-length message to check if the socket is still writable */
    char ender[1] = {};
    int bytes = send(sockfd, ender, 0, MSG_NOSIGNAL);
    try {
        if (bytes == -1)
            throw std::runtime_error("Connection timed out\n");
    } catch (const std::runtime_error &e) {
        if (!Main::get_instance()->GTEST) std::cerr << e.what();
        flags &= ~O_NONBLOCK;
        fcntl(sockfd, F_SETFL, flags);
        return false;
    }

    /* set the socket back to blocking */
    flags &= ~O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);
    return true;
}

int Main::cycle() {
    std::string command;
    std::cin >> command;
    auto req = parse_stdin(command, this);

    /* most advanced try-catch block in poli */
    if (req == nullptr) return 1;
    if (req == (Request*)ERROR_PTR) return 0;

    /* send the request to the server */
    char* message = req->compose_message();
    
    if (are_you_still_there(this->sockfd))
        close_connection(this->sockfd);
    /* with the purpose of having a new connection for each exchange, timeout = 5s */
    this->sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    req->send_message(this->sockfd, message);

    /* receive the response from the server */
    char* response = receive_from_server(this->sockfd);
    Response* res = parse_response(response);

    /* check if the response is an error */
    if (res == (Response*)ERROR_PTR) return 0;

    /* print response code and message, along with body of the response */
    std::cout << res->get_status_code() << " " << res->get_message() << "\n";
    std::cout << res->get_body() << "\n";
    
    res->parse_message(this);
    return 0;
}

int Main::main() {

    this->sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    // std::cerr << "Socket open: " << server_socket << "\n";
    while(true) {
        if (Main::cycle() != 0) break;
    }
    return 0;
}

#include <gtest/gtest.h>

int main(int argc, char** argv) {
    Main *client = Main::get_instance();
    if (argc == 1) {
        client->GTEST = false;
        return client->main();
    }

    /* if there are arguments, assume testing; run gtest */
    /* output will not be shown in test mode */
    client->GTEST = true;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <bits/stdc++.h>
#include <arpa/inet.h>

#include "client.hpp"
#include "compatibility.hpp"

static int server_socket;

int main(void) {

    server_socket = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    std::string command;
    while(true) {
        std::cin >> command;


    }
    return 0;
}

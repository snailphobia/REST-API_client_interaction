#include <bits/stdc++.h>

#include "client.hpp"
#include "compatibility.hpp"

char* GETRequest::compose_message() {
    /* first write cookies to a char*[] typed field (lol archaic code) */
    auto bufcook = this->bonk_cookies();
    
    if (this->get_method() == get) {
        char* msg = compute_get_request((char*)this->get_url().c_str(), (char*)this->get_url().c_str(), NULL, bufcook, this->get_cookies().size());
        for (int i = 0; i < this->get_cookies().size(); i++) {
            free(bufcook[i * 2]);
            free(bufcook[i * 2 + 1]);
        }
        free(bufcook);
        return msg;
    }
}

char* POSTRequest::compose_message() {
    auto bufcook = this->bonk_cookies();

    if (this->get_method() == post) {
        
    }
}

Request* parse_stdin(std::string command) {
    Request* request;
    if (command == "" or command == "exit") return NULL;
    if (command == "login") {
        std::string username, password;
        std::cout << "username: ";
        std::cin >> username;
        std::cout << "password: ";
        std::cin >> password;
    }
    if (command == "logout") {
        std::cerr << "logging out\n";
    }
    if (command == "register") {
        std::string username, password;
        std::cout << "username: ";
        std::cin >> username;
        std::cout << "password: ";
        std::cin >> password;
    }
    if (command == "get_book") {
        std::string id;
        std::cout << "id: ";
        std::cin >> id;
    }
    if (command == "enter_library") {

    }
    if (command == "add_book") {
        std::string title, author, genre, publisher;
        int page_count;
        try {
            std::cout << "title: "; std::cin >> title;
            std::cout << "author: "; std::cin >> author;
            std::cout << "genre: "; std::cin >> genre;
            std::cout << "publisher: "; std::cin >> publisher;
            std::cout << "page_count: "; std::cin >> page_count;
            if (std::cin.fail()) throw std::runtime_error("ai, n-ai bani, te futi in ei\n");
        } catch(const std::runtime_error &e) {
            std::cerr << e.what();
            std::cin.clear();
            return (Request*)ERROR_PTR;
        }
    }
    if (command == "delete_book") {
        std::string id;
        std::cout << "id: ";
        std::cin >> id;
    }
    return (Request*)ERROR_PTR;
}
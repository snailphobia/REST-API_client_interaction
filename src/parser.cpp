#include <bits/stdc++.h>

#include "client.hpp"

char* GETRequest::query_gen() {
    /* if the params vector is empty, return NULL */
    if (this->params.size() == 0) {
        return NULL;
    }

    /* otherwise, generate a query string */
    std::string query;
    for (auto i = 0; i < (int)this->params.size(); i++) {
        query += this->params[i];
        if (i != (int)this->params.size() - 1) {
            query += "&";
        }
    }
    char* query_params = (char*)malloc(query.size() * sizeof(char));
    strcpy(query_params, query.c_str());
    return query_params;
}

char* GETRequest::compose_message() {
    /* first write cookies to a char*[] typed field (lol archaic code) */
    auto bufcook = this->bonk_cookies();
    
    if (this->get_method() == get) {
        char* msg = compute_get_request(SERVER_IP, (char*)this->get_url().c_str(), this->query_gen(), bufcook, this->get_cookies().size());
        /* add JWT if field is not null */
        if (this->get_JWT() != nullptr && this->get_JWT()->first != "") {
            std::string jwt = "Authorization: Bearer " + this->get_JWT()->second + "\r\n\r\n";
            /* will need to replace 2 of the last \r\n with only one */
            std::string msg_stdstr = msg;
            // remove the last 2 characters
            msg_stdstr.pop_back(); msg_stdstr.pop_back();
            msg_stdstr += jwt;
            strcpy(msg, msg_stdstr.c_str());
        }


        for (auto i = 0; i < (int)this->get_cookies().size(); i++) {
            free(bufcook[i * 2]);
            free(bufcook[i * 2 + 1]);
        }
        free(bufcook);
        return msg;
    }
    return nullptr;
}

char* DELRequest::query_gen() {
    /* if the params vector is empty, return NULL */
    if (this->params.size() == 0) {
        return NULL;
    }

    /* otherwise, generate a query string */
    std::string query;
    for (auto i = 0; i < (int)this->params.size(); i++) {
        query += this->params[i];
        if (i != (int)this->params.size() - 1) {
            query += "&";
        }
    }
    char* query_params = (char*)malloc(query.size() * sizeof(char));
    strcpy(query_params, query.c_str());
    return query_params;
}


char* DELRequest::compose_message() {
    std::string msg;
    msg += "DELETE "; msg += this->get_url(); msg += " HTTP/1.1\r\n";
    msg += "Host: "; msg += SERVER_IP; msg += "\r\n";
    if (this->get_JWT() != nullptr)
        msg += "Authorization: Bearer " + this->get_JWT()->second + "\r\n\r\n";
    char* msgc = (char*)malloc(msg.size() * sizeof(char));
    strcpy(msgc, msg.c_str());

    return msgc;
}

char* POSTRequest::compose_message() {
    auto bufcook = this->bonk_cookies();

    if (this->get_method() == post) {
        std::string msg;
        msg += "POST "; msg += this->get_url(); msg += " HTTP/1.1\r\n";
        msg += "Host: "; msg += SERVER_IP; msg += "\r\n";
        msg += "Content-Type: " + (std::string)APP_JSON + "\r\n";
        std::string body = ((JSONify*)this->jsonifier)->to_json(*this->get_body());
        if (this->get_JWT() != nullptr)
            msg += "Authorization: Bearer " + this->get_JWT()->second + "\r\n";
        msg += "Content-Length: " + std::to_string(body.size()) + "\r\n";
        if (this->get_cookies().size() > 0) {
            msg += "Cookie: ";
            for (auto i = 0; i < (int)this->get_cookies().size(); i++) {
                msg += this->get_cookies()[i].first + "=" + this->get_cookies()[i].second;
                if (i != (int)this->get_cookies().size() - 1) msg += "; ";
            }
            msg += "\r\n\r\n";
        }
        else msg += "\r\n";
        msg += body;
        char* msgc = (char*)malloc(msg.size() * sizeof(char));
        strcpy(msgc, msg.c_str());
        return msgc;
    }
    return nullptr;
}

Request* parse_stdin(std::string command, Main* main) {
    Request* request;
    if (command == "" or command == "exit") return nullptr;
    if (command == "login") {
        try {
            if (main->get_session_cookie().first != "") throw std::runtime_error("already logged in\n");
        } catch(std::runtime_error& e) {
            if (!main->GTEST) std::cerr << e.what();
            return (Request*)ERROR_PTR;
        }

        std::string username, password;
        if (!main->GTEST) std::cout << "username: ";
        std::cin >> username;
        if (!main->GTEST) std::cout << "password: ";
        std::cin >> password;

        /* create post request to send username and password */
        request = new POSTRequest(PATH_LOGIN);
        ((POSTRequest*)request)->get_body()->push_back({"username", username});
        ((POSTRequest*)request)->get_body()->push_back({"password", password});

        return request;
    }
    if (command == "logout") {
        if (!main->GTEST) std::cerr << "logging out\n";

        /* create get request to send logout request */
        request = new GETRequest(PATH_LOGOUT);
        request->set_new_cookie("connect.sid", main->get_session_cookie().second);
        main->set_session_cookie({"", ""});
        main->set_JWT_token({"", ""});

        return request;
    }
    if (command == "register") {
        std::string username, password;
        if (!main->GTEST) std::cout << "username: ";
        std::cin >> username;
        if (!main->GTEST) std::cout << "password: ";
        std::cin >> password;

        /* create post request to send username and password */
        request = new POSTRequest(PATH_REG);
        ((POSTRequest*)request)->get_body()->push_back({"username", username});
        ((POSTRequest*)request)->get_body()->push_back({"password", password});

        return request;
    }
    if (command == "get_book") {
        /* JWT required */
        int id;
        if (!main->GTEST) std::cout << "id: ";
        try {
            std::cin >> id;
            if (std::cin.fail()) throw std::runtime_error("la tine si la bani\n");
        } catch (std::runtime_error& e) {
            if (!main->GTEST) std::cerr << e.what();
            std::cin.clear();
            return (Request*)ERROR_PTR;
        }

        /* skull emoji, who would have thought*/
        // std::vector<std::string> wrapper = {std::to_string(id)};
        // request = new GETRequest(PATH_LIB_BOOKS, wrapper);
        request = new GETRequest(std::string(PATH_LIB_BOOKS) + "/" + std::to_string(id));

        try {
            /* if there is no JWT stored */
            if (main->get_session_cookie().first == "" or main->get_session_cookie().second == "") {
                throw std::runtime_error("no JWT found, try something else\n");
            }
            request->set_JWT(main->get_JWT_token().first, main->get_JWT_token().second);
        } catch(std::runtime_error& e) {
            if (!main->GTEST) std::cerr << e.what();
            return (Request*)ERROR_PTR;
        }
        return request;
    }
    if (command == "get_books") {
        /* JWT required */
        request = new GETRequest(PATH_LIB_BOOKS);

        try {
            /* if there is no JWT stored */
            if (main->get_JWT_token().first == "" or main->get_JWT_token().second == "") {
                throw std::runtime_error("\t\tno JWT found, try something else\n");
            }
            request->set_JWT(main->get_JWT_token().first, main->get_JWT_token().second);
        } catch(std::runtime_error& e) {
            if (!main->GTEST) std::cerr << e.what();
            return (Request*)ERROR_PTR;
        }
        return request;
    }
    if (command == "enter_library") {
        /* if there is no connection cookie saved, return with error; */
        if (main->get_session_cookie().first == "" or main->get_session_cookie().second == "") {
            if (!main->GTEST) std::cerr << "\t\tno session cookie found, try something else\n";
            return (Request*)ERROR_PTR;
        }

        request = new GETRequest(PATH_LIB_ACC);
        request->set_new_cookie(main->get_session_cookie().first, main->get_session_cookie().second);

        return request;
    }
    if (command == "add_book") {
        std::string title, author, genre, publisher;
        int page_count;
        try {
            if (!main->GTEST) std::cout << "title: "; std::cin.ignore(); std::getline(std::cin, title);
            if (!main->GTEST) std::cout << "author: "; std::getline(std::cin, author);
            if (!main->GTEST) std::cout << "genre: "; std::getline(std::cin, genre);
            if (!main->GTEST) std::cout << "page_count: "; std::cin >> page_count;
            if (!main->GTEST) std::cout << "publisher: "; std::cin.ignore(); std::getline(std::cin, publisher);
            if (std::cin.fail()) throw std::runtime_error("ai, n-ai bani, te futi in ei\n");
            if (main->get_JWT_token().second == "") throw std::runtime_error("au cea mai perversa shaorma la baneasa\n");
        } catch(const std::runtime_error &e) {
            if (!main->GTEST) std::cerr << e.what();
            std::cin.clear();
            return (Request*)ERROR_PTR;
        }

        request = new POSTRequest(PATH_LIB_BOOKS);
        ((POSTRequest*)request)->get_body()->push_back({"title", title});
        ((POSTRequest*)request)->get_body()->push_back({"author", author});
        ((POSTRequest*)request)->get_body()->push_back({"genre", genre});
        ((POSTRequest*)request)->get_body()->push_back({"page_count", std::to_string(page_count)});
        ((POSTRequest*)request)->get_body()->push_back({"publisher", publisher});

        request->set_JWT(main->get_JWT_token().first, main->get_JWT_token().second);
        return request;
    }
    if (command == "delete_book") {
        int id;
        if (!main->GTEST) std::cout << "id: ";
        try {
            std::cin >> id;
            if (std::cin.fail()) throw std::runtime_error("in drumul meu ma tot gandeam\n");
        } catch(const std::runtime_error &e) {
            if (!main->GTEST) std::cerr << e.what();
            std::cin.clear();
            return (Request*)ERROR_PTR;
        }

        request = new DELRequest(std::string(PATH_LIB_BOOKS) + "/" + std::to_string(id));
        request->set_JWT(main->get_JWT_token().first, main->get_JWT_token().second);

        return request;
    }
    return (Request*)ERROR_PTR;
}

Response* parse_response(char *response) {
    /* parse response */
    std::string response_str(response);
    
    std::string header = response_str.substr(0, response_str.find("\r\n\r\n"));
    std::string body = response_str.substr(response_str.find("\r\n\r\n") + 4, response_str.size() - response_str.find("\r\n\r\n") - 4);
    std::string status = header.substr(header.find("HTTP/1.1 ") + 9, 3);
    std::string status_message = header.substr(header.find("HTTP/1.1 ") + 13, header.find("\r\n") - header.find("HTTP/1.1 ") - 13);

    /* find and store cookies all cookies; line starts with Set-Cookie: */
    std::vector<std::pair<std::string, std::string>> cookies;
    if (header.find("Set-Cookie: ") == std::string::npos) {
        return new Response(status_message, status, body);
        /* no cookies, can skip below code */
    }
    size_t index = header.find("Set-Cookie: ") + 12;
    while (index < header.find("\r\n", index)) {
        std::string cookie = header.substr(index, header.find(";", index) - index);
        if (header.find(";", index) > header.find("\r\n", index)) break;
        std::string cookie_name = cookie.substr(0, cookie.find("="));
        std::string cookie_value = cookie.substr(cookie.find("=") + 1, cookie.find(";") - cookie.find("=") - 1);
        cookies.push_back({cookie_name, cookie_value});
        index = index + cookie.size() + 2;
    }


    /* create response object */
    Response* resp = new Response(status_message, status, body);
    resp->set_cookies(cookies);

    return resp;
}

void Response::parse_message(Main *main) {
    /* check if there is a session cookie in the response */
    for (auto iter : this->get_cookies()) {
        if (iter.first == "connect.sid") {
            main->set_session_cookie(iter);
            break;
        }
    }

    /* check if there is a JWT token in the response */
    auto jsonbody = this->get_json_body();
    for (auto iter : jsonbody) {
        if (iter.first == "token") {
            main->set_JWT_token({"Authorization", iter.second});
            break;
        }
    }
}

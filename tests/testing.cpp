#include <catch2/catch.hpp>
#include "../src/client.hpp"

TEST_CASE("Test Request class") {
    Request request;
    request.set_method(METHOD::GET);
    request.set_url(SERVER_IP);
    request.add_cookie("connect.sid", "12345");
    REQUIRE(request.get_method() == METHOD::GET);
    REQUIRE(request.get_url() == "http://example.com");
    REQUIRE(request.get_cookies().size() == 1);
    REQUIRE(request.get_cookies()[0].first == "session_id");
    REQUIRE(request.get_cookies()[0].second == "12345");
}

TEST_CASE("Test Response class") {
    Response response;
    response.set_status_code(200);
    response.set_body("Hello, world!");
    response.add_header("Content-Type", "text/plain");
    REQUIRE(response.get_status_code() == 200);
    REQUIRE(response.get_body() == "Hello, world!");
    REQUIRE(response.get_headers().size() == 1);
    REQUIRE(response.get_headers()[0].first == "Content-Type");
    REQUIRE(response.get_headers()[0].second == "text/plain");
}
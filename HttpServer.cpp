//
// Copyright (c) 2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP server, small
//
//------------------------------------------------------------------------------

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <iostream>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "Stock.hpp"
#include "Exchange.hpp"
#include "Trade.hpp"
#include <cstdlib>

namespace ip = boost::asio::ip;         // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

using json = nlohmann::json;            // from <nlohmann/json.hpp>

namespace my_program_state
{
    std::size_t
        request_count()
    {
        static std::size_t count = 0;
        return ++count;
    }

    std::time_t
        now()
    {
        return std::time(0);
    }
}

class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
    http_connection(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

    // Initiate the asynchronous operations associated with the connection.
    void
        start()
    {
        read_request();
        check_deadline();
    }

private:
    // The socket for the currently connected client.
    tcp::socket socket_;

    // The buffer for performing reads.
    boost::beast::flat_buffer buffer_{ 8192 };

    // The request message.
    http::request<http::dynamic_body> request_;

    // The response message.
    http::response<http::dynamic_body> response_;

    // The timer for putting a deadline on connection processing.
    boost::asio::basic_waitable_timer<std::chrono::steady_clock> deadline_{
        socket_.get_executor(), std::chrono::seconds(60) };

    // Asynchronously receive a complete request message.
    void
        read_request()
    {
        auto self = shared_from_this();

        http::async_read(
            socket_,
            buffer_,
            request_,
            [self](boost::beast::error_code ec,
                std::size_t bytes_transferred)
            {
                boost::ignore_unused(bytes_transferred);
                if (!ec)
                    self->process_request();
            });
    }

    void send_json_response(http::status status, const json& body) {
        response_.result(status);

        response_.set(
            http::field::content_type,
            "application/json"
        );

        const char* cors_env = std::getenv("CORS_ORIGIN");
        std::string cors_origin = cors_env ? cors_env : "http://localhost:5173";

        response_.set(
            http::field::access_control_allow_origin,
            cors_origin
        );

        response_.set(
            http::field::access_control_allow_methods,
            "GET, POST, OPTIONS"
        );

        response_.set(
            http::field::access_control_allow_headers,
            "Content-Type"
        );

        boost::beast::ostream(response_.body())
            << body.dump();
    }

    std::string extract_id(const std::string& target, const std::string& prefix, const std::string& suffix) {
        if (!target.starts_with(prefix) || !target.ends_with(suffix)) {
            return "";
        }

        std::size_t start_position = prefix.size();
        std::size_t id_length = target.size() - prefix.size() - suffix.size();
        return target.substr(start_position, id_length);
    }

    std::string side_to_string(Side side) {
        switch (side) {
        case Side::BUY:
            return "BUY";

        case Side::SELL:
            return "SELL";
        }

        return "UNKNOWN";
    }

    std::string status_to_string(Status status) {
        switch (status) {
        case Status::OPEN:
            return "OPEN";

        case Status::PARTIALLY_FILLED:
            return "PARTIALLY_FILLED";

        case Status::FILLED:
            return "FILLED";

        case Status::CANCELED:
            return "CANCELED";
        }

        return "UNKNOWN";
    }

    void handle_stocks_request(const std::string& target) {
        if (target.ends_with("/candles")) {
            std::string stock_id = extract_id(target, "/api/stocks/", "/candles");
            Stock* stock = Exchange::get_stock(stock_id);
            if (stock == nullptr) {
                send_json_response(http::status::not_found, { {"error", "Stock not found"} });
                return;
            }
            json candles = json::array();
            for (const auto& [timestamp, candle] : stock->get_candles()) {
                json object = {
                    {"start_time", candle.start_time},
                    {"open", candle.open},
                    {"high", candle.high},
                    {"low", candle.low},
                    {"close", candle.close},
                    {"volume", candle.volume},
                };
                
                candles.push_back(object);
            }

            send_json_response(http::status::ok, candles);
        }

        else if (target.ends_with("/trades")) {
            std::string stock_id = extract_id(target, "/api/stocks/", "/trades");
            Stock* stock = Exchange::get_stock(stock_id);
            if (stock == nullptr) {
                send_json_response(http::status::not_found, { {"error", "Stock not found"} });
                return;
            }

            json trades = json::array();
            for (const auto& trade : stock->get_trade_history()) {
                json object = {
                    {"timestamp", trade.get_executed_timestamp()},
                    {"quantity", trade.get_quantity()},
                    {"price", trade.get_price()}
                };

                trades.push_back(object);
            }

            send_json_response(http::status::ok, trades);
        }

        else if (target.ends_with("/basic_info")) {
            std::string stock_id = extract_id(target, "/api/stocks/", "/basic_info");
            Stock* stock = Exchange::get_stock(stock_id);
            if (stock == nullptr) {
                send_json_response(http::status::not_found, { {"error", "Stock not found"} });
                return;
            }

            auto best_bid = stock->get_order_book().get_best_bid();
            auto best_ask = stock->get_order_book().get_best_ask();

            json basic_info = {
                {"stock_id", stock->get_stock_id()},
                {"stock_name", stock->get_stock_name()},
                {"stock_symbol", stock->get_stock_symbol()},
                {"market_price", stock->get_market_price()},
                {"best_bid", best_bid.has_value() ? json(*best_bid) : json(nullptr)},
                {"best_ask", best_ask.has_value() ? json(*best_ask) : json(nullptr)}
            };

            send_json_response(http::status::ok, basic_info);
        }

        else if (target.ends_with("/stocks")) {
            std::unordered_map<std::string, Stock> stock_map = Exchange::get_stocks();
            json stocks = json::array();
            for (const auto& [k, v] : stock_map) {
                json object = {
                    {"stock_id", v.get_stock_id()},
                    {"stock_name", v.get_stock_name()},
                    {"stock_symbol", v.get_stock_symbol()},
                    {"market_price", v.get_market_price()}
                };

                stocks.push_back(object);
            }

            send_json_response(http::status::ok, stocks);
        }

        else {
            send_json_response(http::status::not_found, { {"error", "Endpoint not found"} });
        }
    }

    void handle_me_request(const std::string& target) {
        // TEMPORARY UNTIL SESSION HANDLING EXISTS
        const std::string user_id = "USER_0";
        User* user = Exchange::get_user(user_id);
        if (user == nullptr) {
            send_json_response(http::status::not_found, { {"error", "User not found"} });
            return;
        }
        
        if (target.ends_with("/me")) {
            json available_stocks = json::array();
            for (const auto& [stock_id, quantity] : user->get_available_stocks()) {
                available_stocks.push_back({ 
                    {"stock_id", stock_id}, 
                    {"quantity", quantity} 
                });
            }

            json reserved_stocks = json::array();
            for (const auto& [stock_id, quantity] : user->get_reserved_stocks()) {
                reserved_stocks.push_back({
                    {"stock_id", stock_id},
                    {"quantity", quantity}
                });
            }

            json object = {
                {"user_id", user->get_user_id()},
                {"username", user->get_username()},
                {"available_cash", user->get_available_cash()},
                {"reserved_cash", user->get_reserved_cash()},
                {"available_stocks", available_stocks},
                {"reserved_stocks", reserved_stocks}
            };

            send_json_response(http::status::ok, object);
        }

        else if (target.ends_with("/me/orders")) {
            json my_orders = json::array();
            for (const auto& order_id : user->get_orders()) {
                Order* order = Exchange::get_order(order_id);
                std::string stock_symbol = Exchange::get_stock(order->get_stock_id())->get_stock_symbol();
                json object = {
                    {"order_id", order_id},
                    {"stock", stock_symbol},
                    {"side", side_to_string(order->get_side())},
                    {"quantity", order->get_quantity()},
                    {"limit_price", order->get_limit_price()},
                    {"status", status_to_string(order->get_status())}
                };

                my_orders.push_back(object);
            }

            send_json_response(http::status::ok, my_orders);
        }

        else {
            send_json_response(http::status::not_found, { {"error", "Endpoint not found"} });
        }
    }

    void handle_orders_request(const std::string& target) {
        if (target == "/api/orders/buy") {
            try {
                std::string body = boost::beast::buffers_to_string(request_.body().data());
                
                json data = json::parse(body);

                if (!data.contains("stock_id") || !data.contains("quantity") || !data.contains("limit_price")) {
                    send_json_response(http::status::bad_request, { {"error", "Missing required fields"} });
                    return;
                }

                std::string stock_id = data["stock_id"].get<std::string>();
                int quantity = data["quantity"].get<int>();
                int limit_price = data["limit_price"].get<int>();

                // Temporary until proper sessions exist
                const std::string user_id = "USER_0";
                User* user = Exchange::get_user(user_id);

                bool success = user->buy_stock(stock_id, quantity, limit_price);

                if (!success) {
                    send_json_response(http::status::bad_request, { {"error", "Buy order could not be placed"} });
                    return;
                }

                send_json_response(http::status::ok, {{"message", "Buy order placed successfully"}});
            }

            catch (const json::exception&) {
                send_json_response(http::status::bad_request, { {"error", "Invalid JSON body"} });
            }
        }

        else if (target == "/api/orders/sell") {
            try {
                std::string body = boost::beast::buffers_to_string(request_.body().data());

                json data = json::parse(body);

                if (!data.contains("stock_id") || !data.contains("quantity") || !data.contains("limit_price")) {
                    send_json_response(http::status::bad_request, { {"error", "Missing required fields"} });
                    return;
                }

                std::string stock_id = data["stock_id"].get<std::string>();
                int quantity = data["quantity"].get<int>();
                int limit_price = data["limit_price"].get<int>();

                // Temporary until proper sessions exist
                const std::string user_id = "USER_0";
                User* user = Exchange::get_user(user_id);

                bool success = user->sell_stock(stock_id, quantity, limit_price);

                if (!success) {
                    send_json_response(http::status::bad_request, { {"error", "Sell order could not be placed"} });
                    return;
                }

                send_json_response(http::status::ok, { {"message", "Sell order placed successfully"} });
            }

            catch (const json::exception&) {
                send_json_response(http::status::bad_request, { {"error", "Invalid JSON body"} });
            }
        }

        else if (target == "/api/orders/cancel") {
            try {
                std::string body = boost::beast::buffers_to_string(request_.body().data());

                json data = json::parse(body);

                if (!data.contains("order_id")) {
                    send_json_response(http::status::bad_request, { {"error", "Missing required fields"} });
                    return;
                }

                std::string order_id = data["order_id"].get<std::string>();
                
                // Temporary until proper sessions exist
                const std::string user_id = "USER_0";
                User* user = Exchange::get_user(user_id);

                bool success = user->cancel_trade(order_id);

                if (!success) {
                    send_json_response(http::status::bad_request, { {"error", "Order could not be cancelled"} });
                    return;
                }

                send_json_response(http::status::ok, { {"message", "Cancel request successful"} });
            }

            catch (const json::exception&) {
                send_json_response(http::status::bad_request, { {"error", "Invalid JSON body"} });
            }
        }

        else {
            send_json_response(http::status::not_found, { {"error", "Endpoint not found"} });
        }
    }

    // Determine what needs to be done with the request message.
    void
        process_request()
    {
        response_.version(request_.version());
        response_.keep_alive(false);

        switch (request_.method())
        {
        case http::verb::get:
            response_.result(http::status::ok);
            response_.set(http::field::server, "Beast");
            create_response();
            break;

        case http::verb::post:
            response_.result(http::status::ok);
            response_.set(http::field::server, "Beast");
            create_response();
            break;

        case http::verb::options:
        {
            response_.result(http::status::no_content);
            const char* cors_env = std::getenv("CORS_ORIGIN");
            std::string cors_origin = cors_env ? cors_env : "http://localhost:5173";

            response_.set(
                http::field::access_control_allow_origin,
                cors_origin
            );

            response_.set(
                http::field::access_control_allow_methods,
                "GET, POST, OPTIONS"
            );

            response_.set(
                http::field::access_control_allow_headers,
                "Content-Type"
            );

            break;
        }

        default:
            // We return responses indicating an error if
            // we do not recognize the request method.
            response_.result(http::status::bad_request);
            response_.set(http::field::content_type, "text/plain");
            boost::beast::ostream(response_.body())
                << "Invalid request-method '"
                << request_.method_string()
                << "'";
            break;
        }

        write_response();
    }

    // Construct a response message based on the program state.
    void create_response()
    {
        std::string target(request_.target());
        std::cout << target << std::endl;
        if (target == "/api/test") {
            send_json_response(http::status::ok, { {"message", "the http server works"} });
        }

        else if (target.starts_with("/api/stocks")) {
            handle_stocks_request(target);
        }

        else if (target.starts_with("/api/me")) {
            handle_me_request(target);
        }

        else if (target.starts_with("/api/orders")) {
            handle_orders_request(target);
        }

        else {
            send_json_response(http::status::not_found, { {"error", "Endpoint not found"} });
        }
    }

    // Asynchronously transmit the response message.
    void
        write_response()
    {
        auto self = shared_from_this();

        response_.content_length(response_.body().size());

        http::async_write(
            socket_,
            response_,
            [self](boost::beast::error_code ec, std::size_t)
            {
                self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                self->deadline_.cancel();
            });
    }

    // Check whether we have spent enough time on this connection.
    void
        check_deadline()
    {
        auto self = shared_from_this();

        deadline_.async_wait(
            [self](boost::beast::error_code ec)
            {
                if (!ec)
                {
                    // Close socket to cancel any outstanding operation.
                    self->socket_.close(ec);
                }
            });
    }
};

// "Loop" forever accepting new connections.
void
http_server(tcp::acceptor& acceptor, tcp::socket& socket)
{
    acceptor.async_accept(socket,
        [&](boost::beast::error_code ec)
        {
            if (!ec)
                std::make_shared<http_connection>(std::move(socket))->start();
            http_server(acceptor, socket);
        });
}


void run_http_server()
{
    try
    {
        auto const address =
            boost::asio::ip::make_address("0.0.0.0");

        const char* port_env = std::getenv("PORT");
        unsigned short port = port_env ? static_cast<unsigned short>(std::stoi(port_env)) : 8080;

        boost::asio::io_context ioc{ 1 };

        tcp::acceptor acceptor{ ioc, {address, port} };
        tcp::socket socket{ ioc };

        http_server(acceptor, socket);

        std::cout << "HTTP server running on port " << port << std::endl;

        ioc.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "HTTP server error: "
            << e.what()
            << '\n';
    }
}
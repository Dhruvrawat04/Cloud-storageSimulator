#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <thread>
#include <iostream>
#include <sstream>
#include <regex>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

namespace httplib {

struct Request {
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
    std::map<std::string, std::string> params;
    
    bool has_param(const std::string& key) const {
        return params.find(key) != params.end();
    }
    
    std::string get_param_value(const std::string& key) const {
        auto it = params.find(key);
        return it != params.end() ? it->second : "";
    }
};

struct Response {
    int status = 200;
    std::map<std::string, std::string> headers;
    std::string body;
    
    void set_header(const std::string& key, const std::string& value) {
        headers[key] = value;
    }
    
    void set_content(const std::string& content, const std::string& content_type) {
        body = content;
        set_header("Content-Type", content_type);
        set_header("Content-Length", std::to_string(content.length()));
    }
};

using Handler = std::function<void(const Request&, Response&)>;

class Server {
private:
    std::map<std::string, std::map<std::string, Handler>> routes;
    int server_fd;
    bool running = false;
    
    void parse_request(const std::string& raw_request, Request& req) {
        std::istringstream stream(raw_request);
        std::string line;
        
        // Parse request line
        if (std::getline(stream, line)) {
            std::istringstream line_stream(line);
            line_stream >> req.method >> req.path;
        }
        
        // Parse headers
        while (std::getline(stream, line) && line != "\r") {
            auto colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string key = line.substr(0, colon_pos);
                std::string value = line.substr(colon_pos + 2);
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
                req.headers[key] = value;
            }
        }
        
        // Parse body
        std::string body_line;
        while (std::getline(stream, body_line)) {
            req.body += body_line + "\n";
        }
        if (!req.body.empty()) {
            req.body.pop_back(); // Remove last newline
        }
    }
    
    std::string build_response(const Response& res) {
        std::ostringstream response;
        response << "HTTP/1.1 " << res.status << " OK\r\n";
        
        for (const auto& header : res.headers) {
            response << header.first << ": " << header.second << "\r\n";
        }
        
        response << "\r\n" << res.body;
        return response.str();
    }
    
    bool match_route(const std::string& pattern, const std::string& path, Request& req) {
        if (pattern == path) {
            return true;
        }
        
        // Handle wildcard patterns like ".*"
        if (pattern == ".*") {
            return true;
        }
        
        // Handle parameter patterns like "/api/files/{id}"
        std::regex pattern_regex(std::regex_replace(pattern, std::regex("\\{[^}]+\\}"), "([^/]+)"));
        std::smatch matches;
        
        if (std::regex_match(path, matches, pattern_regex)) {
            // Extract parameter names from pattern
            std::regex param_regex("\\{([^}]+)\\}");
            std::sregex_iterator iter(pattern.begin(), pattern.end(), param_regex);
            std::sregex_iterator end;
            
            int param_index = 1;
            for (; iter != end; ++iter, ++param_index) {
                std::string param_name = (*iter)[1].str();
                if (param_index < matches.size()) {
                    req.params[param_name] = matches[param_index].str();
                }
            }
            return true;
        }
        
        return false;
    }
    
    void handle_client(int client_socket) {
        char buffer[4096] = {0};
        ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        
        if (bytes_read > 0) {
            Request req;
            parse_request(std::string(buffer, bytes_read), req);
            
            Response res;
            bool handled = false;
            
            // Find matching route
            for (const auto& method_routes : routes) {
                if (method_routes.first == req.method) {
                    for (const auto& route : method_routes.second) {
                        if (match_route(route.first, req.path, req)) {
                            route.second(req, res);
                            handled = true;
                            break;
                        }
                    }
                }
                if (handled) break;
            }
            
            if (!handled) {
                res.status = 404;
                res.set_content("Not Found", "text/plain");
            }
            
            std::string response_str = build_response(res);
            send(client_socket, response_str.c_str(), response_str.length(), 0);
        }
        
        close(client_socket);
    }
    
public:
    void Get(const std::string& pattern, Handler handler) {
        routes["GET"][pattern] = handler;
    }
    
    void Post(const std::string& pattern, Handler handler) {
        routes["POST"][pattern] = handler;
    }
    
    void Delete(const std::string& pattern, Handler handler) {
        routes["DELETE"][pattern] = handler;
    }
    
    void Options(const std::string& pattern, Handler handler) {
        routes["OPTIONS"][pattern] = handler;
    }
    
    bool listen(const std::string& host, int port) {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == 0) {
            std::cerr << "Socket creation failed" << std::endl;
            return false;
        }
        
        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            std::cerr << "Setsockopt failed" << std::endl;
            return false;
        }
        
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        
        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Bind failed" << std::endl;
            return false;
        }
        
        if (::listen(server_fd, 3) < 0) {
            std::cerr << "Listen failed" << std::endl;
            return false;
        }
        
        std::cout << "Server listening on " << host << ":" << port << std::endl;
        running = true;
        
        while (running) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            
            if (client_socket < 0) {
                if (running) {
                    std::cerr << "Accept failed" << std::endl;
                }
                continue;
            }
            
            // Handle client in a separate thread
            std::thread client_thread(&Server::handle_client, this, client_socket);
            client_thread.detach();
        }
        
        return true;
    }
    
    void stop() {
        running = false;
        if (server_fd > 0) {
            close(server_fd);
        }
    }
};

} // namespace httplib
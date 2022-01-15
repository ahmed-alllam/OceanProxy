#ifndef HTTPProxyServer_H
#define HTTPProxyServer_H

#include <utility>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>

class HTTPProxyServer
{
private:
    struct sockaddr_in client_address;
    int client_socket;
    std::string cache_folder;
    std::fstream *log_file;
    std::fstream *blocked_domains_file;

public:
    HTTPProxyServer(struct sockaddr_in client_address, int client_socket,
                    std::string cache_folder, std::fstream *log_file,
                    std::fstream *blocked_domains_file);
    ~HTTPProxyServer();
};

HTTPProxyServer::HTTPProxyServer(struct sockaddr_in client_address, int client_socket,
                                 std::string cache_folder, std::fstream *log_file,
                                 std::fstream *blocked_domains_file)
{
    this->client_address = client_address;
    this->client_socket = client_socket;
    this->cache_folder = cache_folder;
    this->log_file = log_file;
    this->blocked_domains_file = blocked_domains_file;
}

HTTPProxyServer::~HTTPProxyServer()
{
    // TODO: close files and sockets
}

#endif // HTTPProxyServer

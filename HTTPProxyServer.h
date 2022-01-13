#ifndef HTTPProxyServer_H
#define HTTPProxyServer_H

#include <utility>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>



typedef struct ProxyServerSettings {
    struct sockaddr_in client_address;
    int client_socket;
    std::string cache_folder;
    std::fstream* log_file;
    std::fstream* blocked_domains_file;
} ProxyServerSettings;


class HTTPProxyServer
{
private:
    struct sockaddr_in client_address;
    int client_socket;
    std::string cache_folder;
    std::fstream* log_file;
    std::fstream* blocked_domains_file;

public:
    HTTPProxyServer(ProxyServerSettings* server_settings);
    ~HTTPProxyServer();
};

HTTPProxyServer::HTTPProxyServer(ProxyServerSettings* server_settings)
{
    this->client_address = server_settings->client_address;
    this->client_socket = server_settings->client_socket;
    this->cache_folder = server_settings->cache_folder;
    this->log_file = server_settings->log_file;
    this->blocked_domains_file = server_settings->blocked_domains_file;
}

HTTPProxyServer::~HTTPProxyServer()
{
    // TODO: close files and sockets
}

#endif // HTTPProxyServer

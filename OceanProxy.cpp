#include <string>
#include <fstream>
#include <utility>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <argparse/argparse.hpp>

#include "HTTPProxyServer.h"

#define DEFAULT_PORT 8000

struct
{
    int port;
    bool block_plain_HTTP;
    std::string cache_folder;
    std::string log_file;
    std::string blocked_domains_file;
} proxyArgs;

void populateSettingFromParser(argparse::ArgumentParser &parser)
{
    proxyArgs.port = parser.get<int>("port");
    proxyArgs.block_plain_HTTP = parser.get<bool>("-s");
    proxyArgs.cache_folder = parser.get<std::string>("-c");
    proxyArgs.log_file = parser.get<std::string>("-l");
    proxyArgs.blocked_domains_file = parser.get<std::string>("-b");
}

void parseArguments(int argc, char *argv[])
{
    argparse::ArgumentParser parser("OceanProxy");

    parser.add_argument("-p", "--port")
        .help("Port of the HTTP Proxy server")
        .default_value(DEFAULT_PORT)
        .scan<'i', int>();

    parser.add_argument("-s", "--block-plain-http")
        .help("Option to block all unencrypted HTTP traffic")
        .default_value(false)
        .implicit_value(true);

    parser.add_argument("-c", "--cache-folder")
        .default_value(std::string(""))
        .help("Path of the cache folder to use, \
            caching won't be used unless specified");

    parser.add_argument("-l", "--log-file")
        .default_value(std::string(""))
        .help("Path of the file to store the log inside,\
            logging won't be used unless specified");

    parser.add_argument("-b", "--blocked-domains-file")
        .default_value(std::string(""))
        .help("Path of the file to containing list of domians to block, \
            domain filtering won't be used unless specified");

    parser.parse_args(argc, argv);
    populateSettingFromParser(parser);
}

std::fstream *openLogFile()
{
    std::fstream *log_file = nullptr;

    if (!proxyArgs.log_file.empty())
    {
        log_file = new std::fstream();

        log_file->open(proxyArgs.log_file, std::ios::app);

        if (!*(log_file))
        {
            throw "Invalid log file";
        }
    }

    return log_file;
}

std::fstream *openBlockedDomainsFile()
{
    std::fstream *blocked_domains_file = nullptr;

    if (!proxyArgs.blocked_domains_file.empty())
    {
        blocked_domains_file = new std::fstream();

        blocked_domains_file->open(proxyArgs.blocked_domains_file, std::ios::in);

        if (!*(blocked_domains_file))
        {
            throw "Invalid blocked domains file";
        }
    }

    return blocked_domains_file;
}

int initTCPSocket()
{

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1)
    {
        std::cerr << "Error Creating TCP Socket" << std::endl;
        return 3;
    }

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(proxyArgs.port);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr *)&address, sizeof address) < 0)
    {
        std::cerr << "Error Binding the TCP Socket" << std::endl;
        close(socket_fd);
        return 4;
    }

    if (listen(socket_fd, 32) == -1)
    {
        std::cerr << "Error Listening on the TCP Socket" << std::endl;
        close(socket_fd);
        return 4;
    }

    return socket_fd;
}

struct ServerThreadArgs
{
    struct sockaddr_in client_address;
    int client_socket;
    std::string cache_folder;
    std::fstream *log_file;
    std::fstream *blocked_domains_file;
};

void *
initProxyServer(void *args)
{
    struct ServerThreadArgs *thread_args = (ServerThreadArgs *)args;
    HTTPProxyServer *server = new HTTPProxyServer(thread_args->client_address,
                                                  thread_args->client_socket,
                                                  thread_args->cache_folder,
                                                  thread_args->log_file,
                                                  thread_args->blocked_domains_file);
    // server->processRequest();
    return 0;
}

int main(int argc, char *argv[])
{
    std::fstream *log_file;
    std::fstream *blocked_domains_file;

    int socket_fd;

    try
    {
        parseArguments(argc, argv);

        log_file = openLogFile();
        blocked_domains_file = openBlockedDomainsFile();

        socket_fd = initTCPSocket();
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << err.what() << std::endl;
        return 1;
    }
    catch (const char *err)
    {
        std::cerr << err << std::endl;
        return 2;
    }

    while (true)
    {
        socklen_t size;
        struct sockaddr_in client_address;

        int client_socket = accept(socket_fd, (struct sockaddr *)&client_address, &size);
        if (client_socket == -1)
        {
            std::cerr << "Error Accepting the client connection" << std::endl;
            continue;
        }

        struct ServerThreadArgs thread_args = {client_address, client_socket,
                                               proxyArgs.cache_folder, log_file,
                                               blocked_domains_file};

        pthread_t server_thread;
        int thread = pthread_create(&server_thread, NULL, &initProxyServer, &thread_args);

        if (thread != 0)
        {
            std::cerr << "Error Creating server thread" << std::endl;
            close(client_socket);
        }
    }

    close(socket_fd);

    return 0;
}
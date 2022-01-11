#include <string>
#include <fstream>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <argparse/argparse.hpp>

#define DEFAULT_PORT 8000


struct {
    int port;
    bool block_plain_HTTP;
    std::string cache_folder;
    std::string log_file;
    bool debug;
    std::string blocked_domains_file;
} proxySettings;


void populateSettingFromParser(argparse::ArgumentParser &parser) {
    proxySettings.port = parser.get<int>("port");
    proxySettings.block_plain_HTTP = parser.get<bool>("-s");
    proxySettings.cache_folder = parser.get<std::string>("-c");
    proxySettings.log_file = parser.get<std::string>("-l");
    proxySettings.debug = parser.get<bool>("-d");
    proxySettings.blocked_domains_file = parser.get<std::string>("-b");
}

void parseArguments(int argc, char *argv[]) {
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

    
    parser.add_argument("-d", "--debug")
        .help("Print debugging information to the console")
        .default_value(true)
        .implicit_value(true);

    parser.add_argument("-b", "--blocked-domains-file")
        .default_value(std::string(""))
        .help("Path of the file to containing list of domians to block, \
            domain filtering won't be used unless specified");

    parser.parse_args(argc, argv);
    populateSettingFromParser(parser);
}


struct serverThreadArgs {
    struct sockaddr_in client_address;
    int client_socket;

    //TODO: other args
};


void* initProxyServer(void* args) {
    /*HTTPProxyServer server = new HTTPProxyServer(server_args.client_address, 
                                server_args.client_socket, proxySettings);
    server.processRequest(); */

    std::cout << "Connection" << std::endl;
    return nullptr;
}


int main(int argc, char *argv[]) {
    // TO-DO:
    // 1. parse args. Done!
    // 2. open files. Done!
    // 3. open a TCP port (listen). Done!
    // 4. create a new thread (HTTPProxyServer) for each connection. Done!

    try {
        parseArguments(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    
    std::fstream log_file;

    if (!proxySettings.log_file.empty()) {
        log_file.open(proxySettings.log_file, std::ios::app);

        if(!log_file) {
            std::cerr << "Invalid log file" << std::endl;
            return 2;
        }
    }

    std::fstream blocked_domains_file;

    if (!proxySettings.blocked_domains_file.empty()) {
        blocked_domains_file.open(proxySettings.blocked_domains_file, std::ios::in);

        if(!blocked_domains_file) {
            std::cerr << "Invalid blocked domains file" << std::endl;
            return 2;
        }
    }


    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_fd == -1) {
        std::cerr << "Error Creating TCP Socket" << std::endl;
        return 3;
    }


    struct sockaddr_in address; 

    address.sin_family = AF_INET;
    address.sin_port = htons(proxySettings.port);
    address.sin_addr.s_addr = INADDR_ANY;


    if(bind(socket_fd, (struct sockaddr *)&address, sizeof address) < 0) {
        std::cerr << "Error Binding the TCP Socket" << std::endl;
        close(socket_fd);
        return 4;
    }


    if (listen(socket_fd, 32) == -1) {
        std::cerr << "Error Listening on the TCP Socket" << std::endl;
        close(socket_fd);
        return 4;
    }


    while(true) {
        socklen_t size;
        struct sockaddr_in client_address;

        int client_socket = accept(socket_fd, (struct sockaddr *)&client_address, &size);
        if(client_socket == -1) {
            std::cerr << "Error Accepting the client connection" << std::endl;
            continue;
        }

        serverThreadArgs args;
        args.client_address = client_address;
        args.client_socket = client_socket;

        pthread_t server_thread;
        int thread = pthread_create(&server_thread, NULL, &initProxyServer, &args);

        if(thread != 0) {
            std::cerr << "Error Creating server thread" << std::endl;
            close(client_socket);
        }
    }

    close(socket_fd);

    return 0;
}
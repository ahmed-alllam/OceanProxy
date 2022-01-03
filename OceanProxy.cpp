#include <string>
#include <fstream>

#include <argparse/argparse.hpp>


struct {
    int port;
    bool block_plain_HTTP;
    std::string cache_folder;
    std::string log_file;
    std::string blocked_domains_file;
} proxySettings;


void populateSettingFromParser(argparse::ArgumentParser &parser) {
    proxySettings.port = parser.get<int>("port");
    proxySettings.block_plain_HTTP = parser.get<bool>("-s");
    proxySettings.cache_folder = parser.get<std::string>("-c");
    proxySettings.log_file = parser.get<std::string>("-l");
    proxySettings.blocked_domains_file = parser.get<std::string>("-b");
}

void parseArguments(int argc, char *argv[]) {
    argparse::ArgumentParser parser("OceanProxy");

    parser.add_argument("-p", "--port")
        .help("Port of the HTTP Proxy server")
        .default_value(8000)
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


int main(int argc, char *argv[]) {
    // TO-DO:
    // 1. parse args
    // 2. open files
    // 3. open a TCP port (listen)
    // 4. create a new thread (HttpProxyServer) for each connection

    try {
        parseArguments(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    
    if (!proxySettings.log_file.empty()) {
        std::fstream log_file;
        log_file.open(proxySettings.log_file, std::ios::app);

        if(!log_file) {
            std::cerr << "Invalid log file" << std::endl;
            return 2;
        }
    }

    if (!proxySettings.blocked_domains_file.empty()) {
        std::fstream blocked_domains_file;
        blocked_domains_file.open(proxySettings.blocked_domains_file, std::ios::in);

        if(!blocked_domains_file) {
            std::cerr << "Invalid blocked domains file" << std::endl;
            return 2;
        }
    }


    return 0;
}
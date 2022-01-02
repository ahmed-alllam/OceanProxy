#include <iostream>
#include <string>

#include <argparse/argparse.hpp>


struct {
    int port;
    bool blockPlainHTTP;
    std::string cacheFolder;
    std::string logFile;
    std::string blockedDomainsFile;
} proxySettings;


void populateSettingFromParser(argparse::ArgumentParser &parser) {
    proxySettings.port = parser.get<int>("port");
    proxySettings.blockPlainHTTP = parser.get<bool>("-s");
    proxySettings.cacheFolder = parser.get<std::string>("-c");
    proxySettings.logFile = parser.get<std::string>("-l");
    proxySettings.blockedDomainsFile = parser.get<std::string>("-b");
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
        .help("Path of the cache folder to use, caching won't be used unless specified");

    parser.add_argument("-l", "--log-file")
        .default_value(std::string(""))
        .help("Path of the file to store the log inside, logging won't be used unless specified");

    parser.add_argument("-b", "--blocked-domains-file")
        .default_value(std::string(""))
        .help("Path of the file to containing list of domians to block, domain filtering won't be used unless specified");

    try {
        parser.parse_args(argc, argv);
        populateSettingFromParser(parser);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }
}


int main(int argc, char *argv[]) {
    // TO-DO:
    // 1. parse args
    // 2. open a TCP port (listen)
    // 3. create a new thread (HttpProxyServer) for each connection

    parseArguments(argc, argv);
    
    return 0;
}
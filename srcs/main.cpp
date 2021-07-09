
#include "webServer.hpp"
#include "../RequestHandler/RequestHandler.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <iostream>
#include "../classes/ConnectionsHandler/ConnectionsHandler.hpp"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "\x1b[31m" << "ERROR: " << "\x1b[0m" << "may be 1 argv.\n";
        return EXIT_FAILURE;
    }
    parsingConfigFile parsFile(argv[1]);
    try {
        parsFile.parsing();
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    ConnectionsHandler connectionsHandler(parsFile.getListenerConfigVerctor());
    try
    {
        connectionsHandler.startServer();
    }
    catch(const std::exception& e)
    {
        
        std::cerr << "Server finished operation with error: " << std::endl;
        std::cerr << e.what() << std::endl;
        exit (1);
    }
    return 0;
}
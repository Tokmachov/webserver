cmake_minimum_required(VERSION 3.19.2)
project(webServ)

set(CMAKE_CXX_STANDARD 98)
add_compile_options(-g -Wall -Wextra -Werror)
include_directories(classes/CgiHandler)
include_directories(classes/ConnectionsHandler)
include_directories(classes/parseHeader)
include_directories(classes/RequestHandler)
include_directories(classes/Response)
include_directories(classes/ServerCluster)
include_directories(classes/ServerConsts)
include_directories(classes/ServerSettings)
include_directories(classes/Session)
include_directories(utils)
include_directories(includes)


add_executable(webServ  classes/CgiHandler/CgiHandler.cpp
                        classes/CgiHandler/CgiPhp.cpp
                        classes/CgiHandler/CgiTester.cpp
                        classes/ConnectionsHandler/ConnectionsHandler.cpp
                        classes/parseHeader/Request.cpp
                        classes/parseHeader/utils.cpp
                        classes/RequestHandler/RequestHandler.cpp
                        classes/RequestHandler/PathComponents.cpp
                        classes/Response/ErrorResponseFactory.cpp
                        classes/ServerCluster/ServerCluster.cpp
                        classes/ServerConsts/ServerConsts.cpp
                        classes/ServerSettings/parsingConfigFile.cpp
                        classes/ServerSettings/ListenerConfig.cpp
                        classes/Session/Session.cpp
                        srcs/main.cpp
                        utils/_ft_size_tInStr.cpp
                        utils/_ft_strdup.cpp
                        utils/_ft_strjoin.cpp
                        utils/put_log.cpp
                        utils/to_string.cpp
                        utils/writeFileToBuffer.cpp
                        utils/_ft_isdigit.cpp
                        utils/_ft_itoa.cpp)



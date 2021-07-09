/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/22 15:40:23 by anatashi          #+#    #+#             */
/*   Updated: 2021/04/27 14:51:54 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

CgiHandler::CgiHandler() : _env(nullptr),
                           _putFilePath(nullptr),
                           _serverConfig(nullptr),
                           _location(nullptr) {
}

CgiHandler::CgiHandler(const Request *const request,
                       const ListenerConfig *const serverConfig,
                       const t_location *const location,
                       const std::string *const putFilePath,
                       const t_client_addr *const client_addr) : _requestBody(request->getBody()),
                                                                 _env(nullptr),
                                                                 _putFilePath(putFilePath),
                                                                 _serverConfig(serverConfig),
                                                                 _location(location),
                                                                 _fd(-1),
                                                                 _statusCode(0)
                                                                 {
    createTwoDimensionalArrayEnv(request, client_addr);
}

CgiHandler::CgiHandler(const CgiHandler &copy) : _env(nullptr),
                                                 _putFilePath(nullptr),
                                                 _serverConfig(nullptr),
                                                 _location(nullptr) {
    (void)copy;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

CgiHandler::~CgiHandler() {
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

CgiHandler &CgiHandler::operator=(CgiHandler const &rhs) {
    (void)rhs;
    return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

const std::string *CgiHandler::getFile(const Request *const request, const std::string *const putFilePath) {
    (void) request;
    (void) putFilePath;
    _fd = 0;
    return nullptr;
}

const std::string *CgiHandler::getFile(const Request *const request, const t_location *const location) {
    (void) request;
    (void) location;
    _fd = 0;
    return nullptr;
}

// /*
void CgiHandler::parseCGI() {
    size_t endOfHeader = _responseBody.find("\r\n\r\n");
    std::string header = _responseBody.substr(0, endOfHeader);
    size_t i = header.find("Status: ") + 8;
    if (isdigit(header[i])) {
        while (header[i] >= '0' && header[i] <= '9') {
            _statusCode = _statusCode * 10 + (header[i] - '0');
            i++;
        }
        i++;
        size_t j = header.find("\r\n", i) - i;
        _strStatusCode = header.substr(i, j);
        _strContentType = header.substr(j + i + 1, header.size() - 2);
        _responseBody.erase(0, endOfHeader + 4);
    }
}

void CgiHandler::createTwoDimensionalArrayEnv(const Request *const requset, const t_client_addr *const client_addr) {
    _env = new char *[18 + requset->getHeaders().size()];

    _env[0] = _ft_strjoin("AUTH_TYPE=", _location->auth.c_str());
    _env[1] = _ft_strjoin("CONTENT_LENGTH=", to_string(requset->getBody().length()).c_str());
    _env[2] = _ft_strjoin("CONTENT_TYPE=", requset->getValueHeader("content-type").c_str());
    _env[3] = _ft_strdup("GATEWAY_INTERFACE=CGI/1.1");
    _env[4] = _ft_strjoin("PATH_INFO=", _location->location.c_str());
    _env[5] = _ft_strjoin("PATH_TRANSLATED=", std::string(_location->root + _location->location).c_str());
    _env[6] = _ft_strjoin("QUERY_STRING=", requset->getQueryString().c_str());
    _env[7] = _ft_strjoin("REMOTE_ADDR=", client_addr->ip);
    _env[8] = _ft_strjoin("REMOTE_IDENT=", std::string(requset->getUser() + "." + requset->getValueHeader("host")).c_str());
    _env[9] = _ft_strjoin("REMOTE_USER=", requset->getUser().c_str());
    _env[10] = _ft_strjoin("REQUEST_METHOD=", requset->getMethodStr().c_str());
    _env[11] = _ft_strjoin("REQUEST_URI=", _location->location.c_str());
    _env[12] = _ft_strjoin("SCRIPT_NAME=", _location->exec.c_str());
    _env[13] = _ft_strjoin("SERVER_NAME=", _serverConfig->getServerName().c_str());
    char *tmp = _ft_size_tInStr(static_cast<size_t>(_serverConfig->getPort()));
    _env[14] = _ft_strjoin("SERVER_PORT=", tmp);
    delete tmp;
    _env[15] = _ft_strjoin("SERVER_PROTOCOL=", requset->getVersionProtocol().c_str());
    _env[16] = _ft_strdup("SERVER_SOFTWARE=webServ");

    size_t i = 17;
    for (std::map<std::string, std::string>::const_iterator itBegin = requset->getHeaders().cbegin();
         itBegin != requset->getHeaders().end(); itBegin++, ++i) {
        _env[i] = _ft_strjoin("HTTP_", std::string(itBegin->first + "=" + itBegin->second).c_str());
    }
    _env[i] = NULL;
}

bool CgiHandler::executeCgiScript() {

    std::string pathToExecutableFile = _location->root + "/" + _location->exec;

    int status = 0, fds[2], saveSTDIN = 0, saveSTDOUT = 0;
    int buffer_size = 1024, ret = buffer_size;
    char buffer[buffer_size];

    saveSTDIN = dup(0);
    saveSTDOUT = dup(1);

    pipe(fds);
    pid_t idChildProcess = fork();

    dup2(fds[0], 0);
    dup2(fds[1], 1);
    if (!idChildProcess) {
        if (execve(pathToExecutableFile.c_str(), NULL, _env) == -1) {
            close(fds[0]);
            close(fds[1]);
            freeMemory(_env);
            return false;
        }
    } else {
        waitpid(idChildProcess, &status, 0);
        while (ret == buffer_size)
        {
            if ((ret = read(fds[0], buffer, buffer_size)) < 0)
            {
                dup2(saveSTDIN, 0);
                dup2(saveSTDOUT, 1);
                close(fds[0]);
                close(fds[1]);
                freeMemory(_env);
                return false;
            }
            _responseBody += std::string(buffer, ret);
        }
        dup2(saveSTDIN, 0);
        dup2(saveSTDOUT, 1);
        close(fds[0]);
        close(fds[1]);
        parseCGI();
        freeMemory(_env);
    }
    return true;
}

void CgiHandler::freeMemory(char **array) {
    for (size_t i = 0; array[i]; i++) {
        delete array[i];
    }
    delete[] array;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const std::string &CgiHandler::getRequestBody() const {
    return _requestBody;
}

const std::string &CgiHandler::getResponseBody() const {
    return _responseBody;
}

const char *const *CgiHandler::getEnv() const {
    return _env;
}

const int &CgiHandler::getStatusCode() const {
    return _statusCode;
}

const std::string &CgiHandler::getStrStatusCode() const {
    return _strStatusCode;
}

const std::string &CgiHandler::getStrContntentType() const {
    return _strContentType;
}

const ListenerConfig *CgiHandler::getListenerConfig() const {
    return _serverConfig;
}

const char *CgiHandler::FileOpeningErrorException::what() const throw() {
    return ("ERROR!!! The configuration file (max body) contains an invalid character");
}

/* ************************************************************************** */
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiPhp.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/18 14:29:20 by anatashi          #+#    #+#             */
/*   Updated: 2021/04/27 14:44:15 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiPhp.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

CgiPhp::CgiPhp() : CgiHandler() {
}

CgiPhp::CgiPhp(const Request *const request,
               const ListenerConfig *const serverConfig,
               const t_location *const location,
               const std::string *const putFilePath,
               const t_client_addr *const client_addr) : CgiHandler(request,
                                                                    serverConfig,
                                                                    location,
                                                                    putFilePath,
                                                                    client_addr) {
}

CgiPhp::CgiPhp(const CgiPhp &src) {
    (void) src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

CgiPhp::~CgiPhp() {
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

CgiPhp &CgiPhp::operator=(CgiPhp const &rhs) {
    (void) rhs;
    return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void CgiPhp::parseCGI() {
    size_t endOfHeader = _responseBody.find("\r\n\r\n");
    std::string header = _responseBody.substr(0, endOfHeader);
    size_t i = header.find("Status: ") + 8;
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

bool CgiPhp::executeCgiScript() {

    std::string pathToPHP = std::string(_location->root + "/" + _location->exec);
    const char *argvForExecutableFile[3] = {PHP_NAME, pathToPHP.c_str(), NULL};

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

        if (execve(PHP_PATH, (char *const *) argvForExecutableFile, _env) == -1) {
            close(fds[0]);
            close(fds[1]);
            freeMemory(_env);
            return false;
        }
    } else {
        waitpid(idChildProcess, &status, 0);
        while (ret == buffer_size) {
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


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/* ************************************************************************** */
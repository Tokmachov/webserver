/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiTester.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/16 17:52:40 by anatashi          #+#    #+#             */
/*   Updated: 2021/04/18 15:58:53 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiTester.hpp"
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

CgiTester::CgiTester() : CgiHandler() {
}

CgiTester::CgiTester(const Request *const request,
                     const ListenerConfig *const serverConfig,
                     const t_location *const location,
                     const t_client_addr *const client_addr) : CgiHandler(request,
                                                                          serverConfig,
                                                                          location,
                                                                          getFile(request, location),
                                                                          client_addr) {
}

CgiTester::CgiTester(const CgiTester &src) {
    (void) src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

CgiTester::~CgiTester() {
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

CgiTester &CgiTester::operator=(CgiTester const &rhs) {
    (void) rhs;
    return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

std::string CgiTester::makePutFilePathWitNoRoot(const std::string &req_target, const t_location *const location) const {
    std::string location_cpy = location->location, req_target_cpy = req_target;

    if (location_cpy.length() == 1)
        return req_target;
    if (location_cpy[location_cpy.length() - 1] == '/')
        location_cpy.pop_back();
    req_target_cpy.replace(0, location_cpy.length(), "");
    return req_target;
}


std::string CgiTester::getPutRequestFileName(std::string req_target) const {
    size_t slash_position;

    if (req_target[req_target.length() - 1] == '/')
        return std::string("");
    slash_position = req_target.rfind("/", std::string::npos);
    req_target.replace(0, slash_position + 1, "");
    return req_target;
}

const std::string *CgiTester::getFile(const Request *const request, const t_location *const location) {
    std::string put_file_path1(location->root);
    std::string put_file_path_with_no_root = makePutFilePathWitNoRoot(request->getRequestTarget(), location);
    std::string file_name = std::string(getPutRequestFileName(put_file_path_with_no_root));
    put_file_path1 += "/" + file_name;
    return new std::string(put_file_path1);
}

void CgiTester::parseCGI() {
    size_t endOfHeader = _responseBody.find("\r\n\r\n");
    std::string header = _responseBody.substr(0, endOfHeader);
    size_t i = header.find("Status: ") + 8;
    if (_ft_isdigit(header[i])) {
        while (header[i] >= '0' && header[i] <= '9') {
            _statusCode = _statusCode * 10 + (header[i] - '0');
            i++;
        }
        i++;
        size_t j = header.find("\r\n", i) - i;
        _strStatusCode = header.substr(i, j);
        _strContentType = header.substr(j + i + 2, header.size() - 2);
        _responseBody.erase(0, endOfHeader + 4);
    }
}

bool CgiTester::executeCgiScript() {
    std::string pathToExecutableFile = _location->root + "/" + _location->exec;
    const char *argvForExecutableFile[2] = {NULL, NULL};

    int status;

    if ((_fd = open(_putFilePath->c_str(), O_RDONLY)) < 0)
        return false;
    int fd = open(std::string(_location->root + "/tmp.bla").c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
        return false;

    pid_t idChildProcess = fork();
    if (!idChildProcess) {
        dup2(_fd, 0);
        dup2(fd, 1);
        chdir(std::string(_location->root).c_str());
        if (execve(pathToExecutableFile.c_str(), (char *const *) argvForExecutableFile, _env) == -1) {
            close(fd);
            close(_fd);
            freeMemory(_env);
            return false;
        }

    } else {
        waitpid(idChildProcess, &status, 0);
        close(fd);
        close(_fd);
        fd = open(std::string(_location->root + "/tmp.bla").c_str(), O_RDONLY);
        if (writeFileToBuffer(fd, &_responseBody) == -1)
        {
            close(fd);
            unlink(std::string(_location->root + "/tmp.bla").c_str());
            freeMemory(_env);
            delete _putFilePath;
            return false;
        }
        close(fd);
        unlink(std::string(_location->root + "/tmp.bla").c_str());
        parseCGI();
    }
    freeMemory(_env);
    delete _putFilePath;
    return true;
}

/* ************************************************************************** */
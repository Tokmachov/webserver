/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsingConfigFile.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/03 12:23:54 by anatashi          #+#    #+#             */
/*   Updated: 2021/04/12 11:04:57 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSINGCONFIGFILE_HPP
#define PARSINGCONFIGFILE_HPP

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include "webServer.hpp"
#include <algorithm>
#include <vector>
#include <map>
#include "ListenerConfig.hpp"


class parsingConfigFile {
private:
    const char *_argv1;
    std::string _stringToParse;
    std::vector<std::string> _tokensVector;

    std::string _ip;
    int _port;
    std::string _ipAndPort;
    std::string _serverName;
    std::string _errorPage;

    std::vector<ListenerConfig> _ListenerConfigVector;
    std::vector<t_location> _vectorLocations;

    /*
     *  To check for duplicate directives
    */
    bool _isServerListen;
    bool _isServerName;
    bool _isServerErrorPage;
    bool _isServerLocation;
    bool _isLocationAuth;
    bool _isLocationMaxBody;
    bool _isLocationAutoIndex;
    bool _isLocationIndex;
    bool _isLocationMethods;
    bool _isLocationRoot;
    bool _isLocationExec;


    void readingFromFile();

    void splitTheConfigIntoTokens();

    void parsingListenDirective(std::vector<std::string>::const_iterator &itBegin);

    void checkingOctetIp(size_t &indexInString);

    void parsingSpecialDirectivesInLocation(std::vector<std::string>::const_iterator &itBegin, t_location &location);

    void parsingLocationDirective(std::vector<std::string>::const_iterator &itBegin);

    void checkingForMandatoryDirectivesInLocation();

    void checkingForMandatoryDirectivesInServer();

    void parsingMethods(std::vector<std::string>::const_iterator &itBegin, t_location &location);

    void parsingMaxBody(std::vector<std::string>::const_iterator &itBegin, t_location &location);

    void parsingAuth(std::vector<std::string>::const_iterator &itBegin, t_location &location);

    void parsingAutoIndex(std::vector<std::string>::const_iterator &itBegin, t_location &location);

    void setAttributesListenerConfig(ListenerConfig *const server);

    void clearVectorLocations();

    void newElementForTokens(size_t &positon);

    bool ft_isspace(const char &c);

    int ft_atoi(const char *nptr);

    void parsingDirectives(std::vector<std::string>::const_iterator &itBegin);

    void resetDuplicateValueCheckServer();

    void resetDuplicateValueCheckLocation();

    void parsingServerDirective(std::vector<std::string>::const_iterator &itBegin);

    void parsingSpecialServerDirectives(std::vector<std::string>::const_iterator &itBegin);

    template<typename T>
    void checkTocken(std::vector<std::string>::const_iterator &itBegin, const char *bracket, T exception);

    template<typename T>
    void checkDoubleDirective(bool &isValue, T exception);


public:
    parsingConfigFile(const char *argv1);

    ~parsingConfigFile();

    void parsing();

    const std::string &getIp() const;

    const int &getPort() const;

    const std::string &getIpAndPort() const;

    const std::string &getServerName() const;

    const std::vector<ListenerConfig> &getListenerConfigVerctor() const;


    struct UnableToOpenFileException : std::exception {
        const char *what() const throw();
    };

    struct ErrorReadException : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileException : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileExceptionServer : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileExceptionServerNoOpeningParenthesis : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileListenDirectiveException : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileBracketException : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileMethodsException : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileMaxBodyException : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileAuth : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileAutoIndex : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileDoubleDirectiveListen : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileDoubleDirectiveServerName : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileDoubleDirectiveServerErrorPage : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileDoubleDirectiveLocationIndex : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileDoubleDirectiveLocationMethods : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileDoubleDirectiveLocationRoot : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileDoubleDirectiveLocationExec : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileDoubleDirectiveLocationMaxBody : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileDoubleDirectiveLocationAuth : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileDoubleDirectiveLocationAutoIndex : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileThereIsNoMandatoryDirectiveForLocationIndex : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileThereIsNoMandatoryDirectiveForLocationRoot : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileThereIsNoMandatoryDirectiveForLocationMethods : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileThereIsNoMandatoryDirectiveForServerListen : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileThereIsNoMandatoryDirectiveForServerName : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileThereIsNoMandatoryDirectiveForServerErrorPage : std::exception {
        const char *what() const throw();
    };

    struct ErrorInConfigFileThereIsNoMandatoryDirectiveForServerLocation : std::exception {
        const char *what() const throw();
    };
};

#endif
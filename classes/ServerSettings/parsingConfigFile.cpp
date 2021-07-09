
#include "parsingConfigFile.hpp"

parsingConfigFile::parsingConfigFile(const char *argv1) : _argv1(argv1) {
}

parsingConfigFile::~parsingConfigFile() {
}

void parsingConfigFile::readingFromFile() {

    int fd = 0;
    std::cout << _argv1 << std::endl;
    if ((fd = open(_argv1, O_RDONLY)) < 0)
        throw UnableToOpenFileException();
    char buffer[4096];
	//!!! обработать возврат 0 и -1
    while (int ret = read(fd, &buffer, 4096)) {
        if (ret == -1)
            throw ErrorReadException();
        _stringToParse += std::string(buffer, ret);
    }
    close(fd);
}

void parsingConfigFile::splitTheConfigIntoTokens() {
    for (size_t i = 0; i < _stringToParse.size(); i++) {
        if (!ft_isspace(_stringToParse[i]))
            newElementForTokens(i);
    }
}

void parsingConfigFile::clearVectorLocations() {
    _vectorLocations.clear();
}

void parsingConfigFile::parsingListenDirective(std::vector<std::string>::const_iterator &itBegin) {
    size_t colonPositon = (*itBegin).find(':');
    if (colonPositon == std::string::npos) {
        throw ErrorInConfigFileListenDirectiveException();
    }
    size_t indexInString = 0;
    _ip = std::string((*itBegin).begin(), (*itBegin).begin() + colonPositon);
    checkingOctetIp(indexInString);
    _port = ft_atoi(std::string((*itBegin).begin() + colonPositon + 1, (*itBegin).end()).c_str());
    if (_port < 0) {
        throw ErrorInConfigFileListenDirectiveException();
    }
    _ipAndPort = *itBegin;
    _isServerListen = true;
}

void parsingConfigFile::checkingOctetIp(size_t &indexInString) {
    int numOctet = 0;

    for (; _ip[indexInString] >= '0' && _ip[indexInString] <= '9'; indexInString++) {
        numOctet = numOctet * 10 + (_ip[indexInString] - '0');
    }
    if (numOctet > 254 || (_ip[indexInString] != '.' && _ip[indexInString] != '\0'))
        ErrorInConfigFileListenDirectiveException();
    if (_ip[indexInString] == '.')
        checkingOctetIp(++indexInString);
}

void parsingConfigFile::parsingAuth(std::vector<std::string>::const_iterator &itBegin, t_location &location) {
    if (*itBegin == "Basic")
        location.auth = *itBegin;
    else if (*itBegin == "Anonymous")
        return;
    else
        throw ErrorInConfigFileAuth();
}

void parsingConfigFile::parsingAutoIndex(std::vector<std::string>::const_iterator &itBegin, t_location &location) {
    if (*itBegin == "on" || *itBegin == "off")
        location.autoindex = *itBegin;
    else
        throw ErrorInConfigFileAutoIndex();
}


void parsingConfigFile::checkingForMandatoryDirectivesInLocation() {
    if (!_isLocationIndex)
        throw ErrorInConfigFileThereIsNoMandatoryDirectiveForLocationIndex();
    if (!_isLocationRoot)
        throw ErrorInConfigFileThereIsNoMandatoryDirectiveForLocationRoot();
    if (!_isLocationMethods)
        throw ErrorInConfigFileThereIsNoMandatoryDirectiveForLocationMethods();
}

void parsingConfigFile::checkingForMandatoryDirectivesInServer() {
    if (!_isServerListen)
        throw ErrorInConfigFileThereIsNoMandatoryDirectiveForServerListen();
    if (!_isServerName)
        throw ErrorInConfigFileThereIsNoMandatoryDirectiveForServerName();
    if (!_isServerErrorPage)
        throw ErrorInConfigFileThereIsNoMandatoryDirectiveForServerErrorPage();
    if (!_isServerLocation)
        throw ErrorInConfigFileThereIsNoMandatoryDirectiveForServerLocation();

}

void parsingConfigFile::parsingSpecialDirectivesInLocation(std::vector<std::string>::const_iterator &itBegin,
                                                           t_location &location) {
    if (*itBegin == "index") {
        checkDoubleDirective(_isLocationIndex, ErrorInConfigFileDoubleDirectiveLocationIndex());
        location.index = *(++itBegin);
        parsingSpecialDirectivesInLocation(++itBegin, location);
    } else if (*itBegin == "methods") {
        checkDoubleDirective(_isLocationMethods, ErrorInConfigFileDoubleDirectiveLocationMethods());
        parsingMethods(++itBegin, location);
        parsingSpecialDirectivesInLocation(++itBegin, location);
    } else if (*itBegin == "root") {
        checkDoubleDirective(_isLocationRoot, ErrorInConfigFileDoubleDirectiveLocationRoot());
        location.root = *(++itBegin);
        parsingSpecialDirectivesInLocation(++itBegin, location);
    } else if (*itBegin == "exec") {
        checkDoubleDirective(_isLocationExec, ErrorInConfigFileDoubleDirectiveLocationExec());
        location.exec = *(++itBegin);
        parsingSpecialDirectivesInLocation(++itBegin, location);
    } else if (*itBegin == "max_body") {
        checkDoubleDirective(_isLocationMaxBody, ErrorInConfigFileDoubleDirectiveLocationMaxBody());
        parsingMaxBody(++itBegin, location);
        parsingSpecialDirectivesInLocation(++itBegin, location);
    } else if (*itBegin == "auth") {
        checkDoubleDirective(_isLocationAuth, ErrorInConfigFileDoubleDirectiveLocationAuth());
        parsingAuth(++itBegin, location);
        parsingSpecialDirectivesInLocation(++itBegin, location);
    } else if (*itBegin == "autoindex") {
        checkDoubleDirective(_isLocationAutoIndex, ErrorInConfigFileDoubleDirectiveLocationAutoIndex());
        parsingAutoIndex(++itBegin, location);
        parsingSpecialDirectivesInLocation(++itBegin, location);
    }
}

void parsingConfigFile::parsingLocationDirective(std::vector<std::string>::const_iterator &itBegin) {
    resetDuplicateValueCheckLocation();
    t_location location;
    location.auth = "Anonymous";
    location.location = *itBegin;
    location.maxBody = SIZE_MAX;
    location.autoindex = "off";

    checkTocken(++itBegin, "{", ErrorInConfigFileBracketException());
    parsingSpecialDirectivesInLocation(++itBegin, location);
    checkTocken(itBegin, "}", ErrorInConfigFileBracketException());
    checkingForMandatoryDirectivesInLocation();
    _vectorLocations.push_back(location);
}

void parsingConfigFile::parsingMethods(std::vector<std::string>::const_iterator &itBegin, t_location &location) {
    std::vector<std::string> vectorMethods;
    size_t position;
    size_t i;
    size_t countSemicolon;

    // std::cout << *itBegin << "\n";
    for (position = 0, i = 0, countSemicolon = 0; i < (*itBegin).size(); i++) {
        if ((*itBegin)[i] == ',') {
            vectorMethods.push_back((*itBegin).substr(position, i - position));
            position = i + 1;
            countSemicolon++;
        }
        if (countSemicolon > 0 && (i + 1 == (*itBegin).size()))
            vectorMethods.push_back((*itBegin).substr(position, i - position + 1));
        else if (countSemicolon == 0 && (i + 1 == (*itBegin).size()))
            vectorMethods.push_back((*itBegin).substr(position, i - position + 1));
    }

    for (std::vector<std::string>::const_iterator itBeginVectorMethods = vectorMethods.begin();
         itBeginVectorMethods != vectorMethods.end(); itBeginVectorMethods++) {
        if (*itBeginVectorMethods == "GET")
            location.methods.push_back(GET);
        else if (*itBeginVectorMethods == "PUT")
            location.methods.push_back(PUT);
        else if (*itBeginVectorMethods == "POST")
            location.methods.push_back(POST);
        else if (*itBeginVectorMethods == "HEAD")
            location.methods.push_back(HEAD);
        else if (*itBeginVectorMethods == "DELETE")
            location.methods.push_back(DELETE);
        else
            throw ErrorInConfigFileMethodsException();
    }
    location.allowMethods = *itBegin;
}

void parsingConfigFile::parsingMaxBody(std::vector<std::string>::const_iterator &itBegin, t_location &location) {
    for (unsigned int i = 0; (*itBegin)[i]; i++) {
        if (!_ft_isdigit((*itBegin)[i]))
            throw ErrorInConfigFileMaxBodyException();
    }
    location.maxBody = ft_atoi((*itBegin).c_str());
}

void parsingConfigFile::setAttributesListenerConfig(ListenerConfig *const server) {
    server->setPort(static_cast<unsigned int>(_port));
    server->setIp(_ip);
    server->setIpAndPort(_ipAndPort);
    server->setSeverName(_serverName);
    server->setErrorPage(_errorPage);
    server->setVectorLocation(_vectorLocations);
}

bool parsingConfigFile::ft_isspace(const char &c) {
    if (c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' || c == ' ')
        return true;
    return false;
}

int parsingConfigFile::ft_atoi(const char *nptr) {
    int sign;
    int res;
    int i;

    sign = 1;
    res = 0;
    i = 0;
    while (ft_isspace(nptr[i]))
        i++;
    if (nptr[i] == '-' || nptr[i] == '+') {
        if (nptr[i] == '-')
            sign = -1;
        i++;
    }
    while (nptr[i] >= '0' && nptr[i] <= '9') {
        res = res * 10 + (nptr[i] - '0');
        i++;
    }
    return (res * sign);
}

void parsingConfigFile::newElementForTokens(size_t &position) {
    size_t i = position;
    while (!ft_isspace(_stringToParse[i]) && _stringToParse[i]) {
        i++;
    }
    _tokensVector.push_back(_stringToParse.substr(position, i - position));
    position = i;
}

void parsingConfigFile::parsing() {
    readingFromFile();
    splitTheConfigIntoTokens();
    std::vector<std::string>::const_iterator itBegin = _tokensVector.begin();
    parsingDirectives(itBegin);
}

template<typename T>
void parsingConfigFile::checkTocken(std::vector<std::string>::const_iterator &itBegin, const char *bracket,
                                    T exception) {
    if (*itBegin != bracket)
        throw exception;
}

template<typename T>
void parsingConfigFile::checkDoubleDirective(bool &isValue, T exception) {
    if (isValue)
        throw exception;
    else
        isValue = true;
}

void parsingConfigFile::parsingServerDirective(std::vector<std::string>::const_iterator &itBegin) {
    checkTocken(itBegin, "server", ErrorInConfigFileExceptionServer());

}

void parsingConfigFile::parsingSpecialServerDirectives(std::vector<std::string>::const_iterator &itBegin) {
    if (*itBegin == "listen") {
        checkDoubleDirective(_isServerListen, ErrorInConfigFileDoubleDirectiveListen());
        parsingListenDirective(++itBegin);
        parsingSpecialServerDirectives(++itBegin);
    } else if (*itBegin == "server_name") {
        checkDoubleDirective(_isServerName, ErrorInConfigFileDoubleDirectiveServerName());
        _serverName = *(++itBegin);
        parsingSpecialServerDirectives(++itBegin);
    } else if (*itBegin == "error_page") {
        checkDoubleDirective(_isServerErrorPage, ErrorInConfigFileDoubleDirectiveServerErrorPage());
        _errorPage = *(++itBegin);
        parsingSpecialServerDirectives(++itBegin);
    } else if (*itBegin == "location") {
        parsingLocationDirective(++itBegin);
        _isServerLocation = true;
        parsingSpecialServerDirectives(++itBegin);
    }
}


void parsingConfigFile::resetDuplicateValueCheckServer() {
    _isServerListen = false;
    _isServerName = false;
    _isServerErrorPage = false;
    _isServerLocation = false;
}

void parsingConfigFile::resetDuplicateValueCheckLocation() {
    _isLocationAuth = false;
    _isLocationMaxBody = false;
    _isLocationAutoIndex = false;
    _isLocationIndex = false;
    _isLocationMethods = false;
    _isLocationRoot = false;
    _isLocationExec = false;
}

void parsingConfigFile::parsingDirectives(std::vector<std::string>::const_iterator &itBegin) {

    resetDuplicateValueCheckServer();
    parsingServerDirective(itBegin);
    checkTocken(++itBegin, "{", ErrorInConfigFileExceptionServerNoOpeningParenthesis());

    parsingSpecialServerDirectives(++itBegin);
    checkTocken(itBegin, "}", ErrorInConfigFileExceptionServerNoOpeningParenthesis());

    checkingForMandatoryDirectivesInServer();
    ListenerConfig server;
    setAttributesListenerConfig(&server);
    clearVectorLocations();
    _ListenerConfigVector.push_back(server);
    std::cout << server << "\n";
    if (++itBegin != _tokensVector.end())
        parsingDirectives(itBegin);
}

const std::string &parsingConfigFile::getIp() const {
    return _ip;
}

const int &parsingConfigFile::getPort() const {
    return _port;
}

const std::string &parsingConfigFile::getIpAndPort() const {
    return _ipAndPort;
}

const std::string &parsingConfigFile::getServerName() const {
    return _serverName;
}

const std::vector<ListenerConfig> &parsingConfigFile::getListenerConfigVerctor() const {
    return _ListenerConfigVector;
}



const char *parsingConfigFile::UnableToOpenFileException::what() const throw() {
    return ("ERROR!!! The configuration settings file cannot be opened. Check the name of the file or its location");
}

const char *parsingConfigFile::ErrorReadException::what() const throw() {
    return ("ERROR!!! Error reading from the configuration file");
}

const char *parsingConfigFile::ErrorInConfigFileExceptionServer::what() const throw() {
    return ("ERROR!!! There is no server directive in the configuration file");
}

const char *parsingConfigFile::ErrorInConfigFileExceptionServerNoOpeningParenthesis::what() const throw() {
    return ("ERROR!!! The server description should start with an opening parenthesis {");
}

const char *parsingConfigFile::ErrorInConfigFileException::what() const throw() {
    return ("ERROR!!! Parameter error in the configuration file");
}

const char *parsingConfigFile::ErrorInConfigFileListenDirectiveException::what() const throw() {
    return ("ERROR!!! An error was found in the listen directive");
}

const char *parsingConfigFile::ErrorInConfigFileBracketException::what() const throw() {
    return ("ERROR!!! The configuration file is missing the closing brackets");
}

const char *parsingConfigFile::ErrorInConfigFileMethodsException::what() const throw() {
    return ("ERROR!!! The configuration file contains unsupported methods");
}

const char *parsingConfigFile::ErrorInConfigFileMaxBodyException::what() const throw() {
    return ("ERROR!!! The configuration file (max body) contains an invalid character");
}

const char *parsingConfigFile::ErrorInConfigFileAuth::what() const throw() {
    return ("ERROR!!! The configuration file (auth) contains an invalid character");
}

const char *parsingConfigFile::ErrorInConfigFileAutoIndex::what() const throw() {
    return ("ERROR!!! The configuration file (authindex) mayby only #on# or #off#");
}

const char *parsingConfigFile::ErrorInConfigFileDoubleDirectiveListen::what() const throw() {
    return ("ERROR!!! Duplicate the listen directive");
}

const char *parsingConfigFile::ErrorInConfigFileDoubleDirectiveServerName::what() const throw() {
    return ("ERROR!!! Duplicate the server_name directive");
}

const char *parsingConfigFile::ErrorInConfigFileDoubleDirectiveServerErrorPage::what() const throw() {
    return ("ERROR!!! Duplicate the error_page directive");
}

const char *parsingConfigFile::ErrorInConfigFileDoubleDirectiveLocationIndex::what() const throw() {
    return ("ERROR!!! Duplicate the index directive inside location");
}

const char *parsingConfigFile::ErrorInConfigFileDoubleDirectiveLocationMethods::what() const throw() {
    return ("ERROR!!! Duplicate the methods directive inside location");
}

const char *parsingConfigFile::ErrorInConfigFileDoubleDirectiveLocationRoot::what() const throw() {
    return ("ERROR!!! Duplicate the root directive inside location");
}

const char *parsingConfigFile::ErrorInConfigFileDoubleDirectiveLocationExec::what() const throw() {
    return ("ERROR!!! Duplicate the exec directive inside location");
}

const char *parsingConfigFile::ErrorInConfigFileDoubleDirectiveLocationMaxBody::what() const throw() {
    return ("ERROR!!! Duplicate the max_body directive inside location");
}

const char *parsingConfigFile::ErrorInConfigFileDoubleDirectiveLocationAuth::what() const throw() {
    return ("ERROR!!! Duplicate the auth directive inside location");
}

const char *parsingConfigFile::ErrorInConfigFileDoubleDirectiveLocationAutoIndex::what() const throw() {
    return ("ERROR!!! Duplicate the autoindex directive inside location");
}

const char *parsingConfigFile::ErrorInConfigFileThereIsNoMandatoryDirectiveForLocationIndex::what() const throw() {
    return ("ERROR!!! There is no mandatory index directive for location");
}

const char *parsingConfigFile::ErrorInConfigFileThereIsNoMandatoryDirectiveForLocationRoot::what() const throw() {
    return ("ERROR!!! There is no mandatory root directive for location");
}

const char *parsingConfigFile::ErrorInConfigFileThereIsNoMandatoryDirectiveForLocationMethods::what() const throw() {
    return ("ERROR!!! There is no mandatory method directive for location");
}

const char *parsingConfigFile::ErrorInConfigFileThereIsNoMandatoryDirectiveForServerListen::what() const throw() {
    return ("ERROR!!! There is no mandatory listen directive for server");
}

const char *parsingConfigFile::ErrorInConfigFileThereIsNoMandatoryDirectiveForServerName::what() const throw() {
    return ("ERROR!!! There is no mandatory name directive for location");
}

const char *parsingConfigFile::ErrorInConfigFileThereIsNoMandatoryDirectiveForServerErrorPage::what() const throw() {
    return ("ERROR!!! There is no mandatory error_page directive for location");
}

const char *parsingConfigFile::ErrorInConfigFileThereIsNoMandatoryDirectiveForServerLocation::what() const throw() {
    return ("ERROR!!! There is no mandatory Location directive for location");
}
#include "RequestHandler.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <bitset>
#include <fstream>

#define reset "\x1b[0m"
#define cyan "\x1b[36m"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
RequestHandler::RequestHandler() {
}

RequestHandler::RequestHandler(const ServerCluster &serverCluster, t_client_addr addr)
        : _serverCluster(serverCluster), _client_addr(addr) 
{
}

RequestHandler::RequestHandler(const RequestHandler &src) {
    _serverCluster = src._serverCluster;
    _serverConfig = src._serverConfig;
    _client_addr = src._client_addr;
    _location = src._location;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

RequestHandler::~RequestHandler() {
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

RequestHandler &RequestHandler::operator=(RequestHandler const &rhs) {
    if (this != &rhs) {
        _serverCluster = rhs._serverCluster;
        _serverConfig = rhs._serverConfig;
        _client_addr = rhs._client_addr;
        _location = rhs._location;
    }
    return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

Response RequestHandler::handleRequest(Request &req) {
    Response resp;

    if (_isRequestParsedCorrectly(resp, req) == false) {
        std::cout << "is bad req" << std::endl;
        if (req.getMethodEnum() == HEAD) {
            resp.body = "";
            resp.headers["content-length"] = "0";
            resp.target = req.getRequestTarget();
        }
        return resp;
    }
    if (_isHostHeaderPresent(resp, req) == false) {
        std::cout << "is bad host\n";
        if (req.getMethodEnum() == HEAD) {
            resp.body = "";
            resp.headers["content-length"] = "0";
            resp.target = req.getRequestTarget();
        }
        return resp;
    }
    _serverConfig = _choseServerConfig(req.getValueHeader("host"));
    if (_choseLocation(&_location, req.getRequestTarget()) == false) {
        resp = ErrorResponseFactory::create(ErrorResponseFactory::NOT_FOUND, _serverConfig.getErrorPageDir());
        if (req.getMethodEnum() == HEAD) {
            resp.body = "";
            resp.headers["content-length"] = "0";
            resp.target = req.getRequestTarget();
        }
        return resp;
    }
    if (_isStatusLineValid(resp, req) == false) {
        std::cout << "is bad status line\n";
        if (req.getMethodEnum() == HEAD) {
            resp.body = "";
            resp.headers["content-length"] = "0";
            resp.target = req.getRequestTarget();
        }
        return resp;
    }
    _pathComponents =  PathComponents
    (
        req.getRequestTarget(), 
        _location.root, 
        _location.index, 
        std::string(_location.location).length()
    );
    if (_pathComponents.getPathType() == PathComponents::DIR && _location.autoindex == "on" && req.getMethodEnum() == GET) {
        if (_isTerminatedWithSlash(req.getRequestTarget()) == false) {
            resp = _makeAddSlashToDirResponse(req.getRequestTarget());
            return resp;
        }
        resp = _makeDirectoryListingPageResponse(_pathComponents.getCurrentPath());
        return resp;
    }
    switch (req.getMethodEnum()) {
        case HEAD:
            _handleHeadRequest(resp, req);
            break;
        case GET:
            _handleGetRequest(resp, req);
            break;
        case PUT:
            _handlePutRequest(resp, req);
            break;
        case POST:
            _handlePostRequest(resp, req);
            break;
        case DELETE:
            _handleDeleteRequest(resp, req);
        case UNSUPPORTED:
            break;
    }
    return resp;
}

/*
** --------------------------------- PRIVATE METHODS ---------------------------
*/

bool RequestHandler::_isRequestParsedCorrectly(Response &resp, const Request &req) const {
    if (req.getIsBadRequest() == true) {
        resp = ErrorResponseFactory::create(
                ErrorResponseFactory::BAD_REQUEST,
                _serverCluster._configs[0].getErrorPageDir());
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
bool RequestHandler::_isHostHeaderPresent(Response &resp, const Request &req) const {
    std::string error_page_dir;

    error_page_dir = _serverCluster._configs[0].getErrorPageDir();
    if (req.getValueHeader("host").empty() == true) {
        resp = ErrorResponseFactory::create(ErrorResponseFactory::BAD_REQUEST, error_page_dir);
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
ListenerConfig RequestHandler::_choseServerConfig(const std::string &host) const {
    size_t i;

    for (i = 0; i < _serverCluster._configs.size(); i++) {
        if (host == _serverCluster._configs[i].getServerName())
            return _serverCluster._configs[i];
    }
    return _serverCluster._configs[0];
}

//------------------------------------------------------------------------------
bool RequestHandler::_choseLocation(t_location *location, const std::string &req_target) const {
    std::vector<t_location>::const_iterator it_begin;
    std::vector<t_location>::const_iterator it_end;
    bool is_location_chosen;

    it_begin = _serverConfig.getVectorLocation().begin();
    it_end = _serverConfig.getVectorLocation().end();
    is_location_chosen = false;
    location->location = "";
    for (; it_begin < it_end; it_begin++) {
        if (_isSubpath(it_begin->location, req_target) && it_begin->location > location->location) {
            is_location_chosen = true;
            *location = *it_begin;
        }
    }
    return is_location_chosen;
}

//------------------------------------------------------------------------------
bool RequestHandler::_isSubpath(std::string subpath, const std::string &path) const {
    size_t subpathEndIndex;

    subpathEndIndex = subpath.length();
    if (subpath[subpathEndIndex - 1] == '/') {
        subpath.erase(subpathEndIndex - 1);
        subpathEndIndex -= 1;
    }
    if ((path.find(subpath) == 0) && (path[subpathEndIndex] == '/' || path[subpathEndIndex] == '\0'))
        return true;
    return false;
}
//------------------------------------------------------------------------------

bool RequestHandler::_isStatusLineValid(Response &resp, const Request &req) const {
    std::string error_page_dir;

    error_page_dir = _serverConfig.getErrorPageDir();
    if (_isMethodValidForServer(req) == false) {
        resp = ErrorResponseFactory::create(
                ErrorResponseFactory::NOT_IMPLEMENTED_METHOD,
                error_page_dir);
        return false;
    }
    if (_isMethodImplementedForLocation(req) == false) {
        resp = ErrorResponseFactory::create(
                ErrorResponseFactory::METHOD_NOT_ALLOWED,
                error_page_dir,
                _location.methods);
        return false;
    }
    if (_isProtocolValid(req) == false) {
        resp = ErrorResponseFactory::create(
                ErrorResponseFactory::VERSION_NOT_SUPPORTED,
                error_page_dir);
        return false;
    }
    return true;
}

bool RequestHandler::_isMethodValidForServer(const Request &req) const {
    std::vector<Method> methods = ServerConsts::getImplementedMethods();
    std::vector<Method>::const_iterator begin_it = methods.cbegin();
    std::vector<Method>::const_iterator end_it = methods.cend();

    if (find(begin_it, end_it, req.getMethodEnum()) != end_it)
        return true;
    return false;
}

bool RequestHandler::_isMethodImplementedForLocation(const Request &req) const {
    std::vector<Method>::const_iterator it_begin = _location.methods.begin();
    std::vector<Method>::const_iterator it_end = _location.methods.end();

    for (; it_begin != it_end; it_begin++) {
        if (req.getMethodEnum() == *it_begin)
            return true;
    }
    return false;
}

bool RequestHandler::_isProtocolValid(const Request &req) const {
    if (req.getVersionProtocol() != "HTTP/1.1")
        return false;
    return true;
}

std::string RequestHandler::_getPagePath(const Request &req) {
    std::string reqTarget;
    std::string pagePath;

    reqTarget = req.getRequestTarget();
    if (reqTarget.length() > 1 && reqTarget[reqTarget.length() - 1] == '/') {
        reqTarget.pop_back();
    }
    if (reqTarget == "/400_bad_requset/images/404_error.png" ||
        reqTarget == "/404_not_found/images/404_error.png" ||
        reqTarget == "/501_not_implemented/images/404_error.png") {
        pagePath = this->_serverConfig.getErrorPageDir() + reqTarget;
        return (pagePath);
    } else if (reqTarget == this->_location.location)
        pagePath = this->_location.root + "/" + this->_location.index;
    else {
        if (this->_location.location == "/")
            pagePath = this->_location.root + reqTarget;
        else
            pagePath = this->_location.root + reqTarget.substr(this->_location.location.length());
    }
    DIR *dir = opendir(pagePath.c_str());
    if (dir) {
        pagePath += "/" + this->_location.index;
        closedir(dir);
    }
    return (pagePath);
}

char *getTime(char *mbstr, const char *format, const char *pagePath, bool lm) {
    struct stat buf;
    int result;
    std::time_t t = std::time(NULL);

    if (lm) {
        result = stat(pagePath, &buf);
        if (result == 0) {
            if (std::strftime(mbstr, sizeof(char) * 100, format, std::gmtime(&buf.st_ctimespec.tv_sec))) {
                return (mbstr);
            }
        }
    } else {
        if (std::strftime(mbstr, sizeof(char) * 100, format, std::gmtime(&t))) {
            return (mbstr);
        }
    }
    memset(mbstr, '\0', strlen(mbstr));
    return (mbstr);
}

void RequestHandler::_handleGetRequest(Response &r, const Request &req) {
    std::string pagePath;
    int fd;
    int found;
    std::string resFile;
    char mbstr[100];

    r.method = GET;
    pagePath = this->_getPagePath(req);
    if (req.getValueHeader("authorization") != "") {
        if ("test:test" != req.getUserAndPass()) {
            r.status_code_int_val = 401;
            r.reason_phrase = "Unauthorized";
            r.addHeader("Date", getTime(mbstr, "%a, %d %b %Y %H:%M:%S GMT", pagePath.c_str(), false));
            return;
        }
    } else if (this->_location.auth != "Anonymous") {
        r.status_code_int_val = 401;
        r.reason_phrase = "Unauthorized";
        r.addHeader("WWW-Authenticate", "Basic realm=\"test\"");
        r.addHeader("Date", getTime(mbstr, "%a, %d %b %Y %H:%M:%S GMT", pagePath.c_str(), false));
        return;
    }
    fd = open(pagePath.c_str(), O_RDONLY);
    if (fd == -1) {
        r = ErrorResponseFactory::create(ErrorResponseFactory::NOT_FOUND, _serverConfig.getErrorPageDir());
        return;
    }
    if (writeFileToBuffer(fd, &r.body) == -1)
    {
        r = ErrorResponseFactory::create(ErrorResponseFactory::INTERNAL_SERVER_ERROR, _serverConfig.getErrorPageDir());
        return ;
    }
    close(fd);

    r.status_code_int_val = 200;
    r.reason_phrase = "OK";
    found = pagePath.rfind(".");
    resFile = pagePath.substr(found + 1);
    if (resFile == "html")
        r.addHeader("Content-Type", "text/html");
    else if (resFile == "png")
        r.addHeader("Content-Type", "image/png");
    else if (resFile == "ico")
        r.addHeader("Content-Type", "image/x-icon");
    else
        r.addHeader("Content-Type", "text/plain");
    r.addHeader("Date", getTime(mbstr, "%a, %d %b %Y %H:%M:%S GMT", pagePath.c_str(), false));
    r.addHeader("Content-Length", to_string(r.body.length()));
    r.addHeader("Last-Modified", getTime(mbstr, "%a, %d %b %Y %H:%M:%S GMT", pagePath.c_str(), true));
    r.target = req.getRequestTarget();
}

//------------------------------------------------------------------------------
void RequestHandler::_handleHeadRequest(Response &r, const Request &req) {
    std::string pagePath;
    int fd;
    int found;
    std::string resFile;
    char mbstr[100];

    r.method = HEAD;
    pagePath = this->_getPagePath(req);
    if (req.getValueHeader("authorization") != "") {
        if ("test:test" != req.getUserAndPass()) {
            r.status_code_int_val = 401;
            r.reason_phrase = "Unauthorized";
            r.addHeader("Date", getTime(mbstr, "%a, %d %b %Y %H:%M:%S GMT", pagePath.c_str(), false));
            r.target = req.getRequestTarget();
            return;
        }
    } else if (this->_location.auth != "Anonymous") {
        r.status_code_int_val = 401;
        r.reason_phrase = "Unauthorized";
        r.addHeader("WWW-Authenticate", "Basic realm=\"test\"");
        r.addHeader("Date", getTime(mbstr, "%a, %d %b %Y %H:%M:%S GMT", pagePath.c_str(), false));
        r.target = req.getRequestTarget();
        return;
    }
    fd = open(pagePath.c_str(), O_RDONLY);
    if (fd == -1) {
        r = ErrorResponseFactory::create(ErrorResponseFactory::NOT_FOUND, _serverConfig.getErrorPageDir());
        r.target = req.getRequestTarget();
        return;
    }
    close(fd);

    r.status_code_int_val = 200;
    r.reason_phrase = "OK";
    found = pagePath.rfind(".");
    resFile = pagePath.substr(found + 1);
    if (resFile == "html")
        r.addHeader("Content-Type", "text/html");
    else if (resFile == "png")
        r.addHeader("Content-Type", "image/png");
    else if (resFile == "ico")
        r.addHeader("Content-Type", "image/x-icon");
    else
        r.addHeader("Content-Type", "text/plain");
    r.addHeader("Date", getTime(mbstr, "%a, %d %b %Y %H:%M:%S GMT", pagePath.c_str(), false));
    r.addHeader("Content-Length", to_string(r.body.length()));
    r.addHeader("Last-Modified", getTime(mbstr, "%a, %d %b %Y %H:%M:%S GMT", pagePath.c_str(), true));
    r.target = req.getRequestTarget();
}
//------------------------------------------------------------------------------
#define RESULT_OPENED 1
#define RESULT_CREATED 0
#define RESULT_FAILED -1

void RequestHandler::_handlePutRequest(Response &resp, const Request &req) 
{
    int open_file_result;
    int fd;
    int write_result;
    
    if (req.getBody().length() > _location.maxBody) 
    {
        resp = ErrorResponseFactory::create(ErrorResponseFactory::PAYLOAD_TO_LARGE);
        return;
    }
    if (_pathComponents.getPathType() == PathComponents::DIR 
        || _pathComponents.getPathType() == PathComponents::NON_EXISTING_DIR)
    {
        resp = ErrorResponseFactory::create(
                ErrorResponseFactory::BAD_REQUEST,
                _serverConfig.getErrorPageDir());
        return;
    }
    _openOrCreateDestFile(&open_file_result, &fd, _pathComponents.getCurrentPath());
    if (open_file_result == RESULT_FAILED) 
    {
        resp = ErrorResponseFactory::create(
                ErrorResponseFactory::INTERNAL_SERVER_ERROR,
                _serverConfig.getErrorPageDir());
        return;
    }
    write_result = write(fd, req.getBody().c_str(), req.getBody().length());
    if (write_result < (long long) req.getBody().length()) 
    {
        resp = ErrorResponseFactory::create(
                ErrorResponseFactory::INTERNAL_SERVER_ERROR,
                _serverConfig.getErrorPageDir());
        return;
    }
    close(fd);
    resp = _makePutRequestResponse(open_file_result, _pathComponents.getCurrentPath());
}

void RequestHandler::_openOrCreateDestFile(int *result, int *fd, std::string path) const {
    *fd = open(path.c_str(), O_RDWR | O_TRUNC, S_IRWXU);
    if (*fd >= 0) {
        *result = RESULT_OPENED;
        return;
    }
    *fd = open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    if (*fd >= 0) {
        *result = RESULT_CREATED;
        return;
    }
    *result = RESULT_FAILED;
}

Response RequestHandler::_makePutRequestResponse(int open_file_result, std::string file_path) const {
    Response resp;
    std::string lastModifiedHeaderValue;

    if (open_file_result == RESULT_CREATED) {
        resp.status_code_int_val = 201;
        resp.reason_phrase = std::string("Created");
        resp.addHeader("content-length", "0");
    } else if (open_file_result == RESULT_OPENED) {
        resp.status_code_int_val = 200;
        resp.reason_phrase = std::string("Modified");
        resp.addHeader("content-length", "0");
    }
    lastModifiedHeaderValue = _createLastModifiedHeaderValue(file_path);
    resp.addHeader("Last-Modified", lastModifiedHeaderValue);
    return resp;
}

std::string RequestHandler::_createLastModifiedHeaderValue(const std::string &file_path) const {
    int result;
    struct stat file_stat;
    std::time_t time_modified;
    std::tm *time_struct;
    char time_modified_str[100];

    result = stat(file_path.c_str(), &file_stat);
    if (result != 0)
        return "";
    time_modified = file_stat.st_mtime;
    time_struct = std::localtime(&time_modified);
    std::strftime(time_modified_str, sizeof(time_modified_str), "%a, %d %b %Y %H:%M:%S GMT", time_struct);
    return std::string(time_modified_str);
}

//------------------------------------------------------------------------------


CgiHandler *RequestHandler::Cgi(const Request &request, const std::string &put_file_path) {
    std::string execFile = std::string(_location.root + "/" + _location.exec);
    int fdCgi = open(execFile.c_str(), O_RDONLY);
    if (_location.exec.empty() || fdCgi == -1) {
        close(fdCgi);
        return nullptr;
    }
    close(fdCgi);
    if (_location.exec == "cgi_tester")
        return new CgiTester(&request, &_serverConfig, &_location, &_client_addr);
    else if (_location.exec.find(".php") != std::string::npos)
        return new CgiPhp(&request, &_serverConfig, &_location, &put_file_path, &_client_addr);
    return new CgiHandler(&request, &_serverConfig, &_location, &put_file_path, &_client_addr);
}

void RequestHandler::_handlePostRequest(Response &r, const Request &req) {
    std::string put_file_path;

    if (req.getBody().length() > _location.maxBody) {
        r.body = "";
        r.status_code_int_val = 413;
        r.reason_phrase = "Payload Too Large";
        r.addHeader("Content-Length", "0");
    }
    else if (req.getBody().length() == _location.maxBody || (req.getBody().length() == 0 && req.getQueryString().empty()))
    {
        r.body = req.getBody();
        r.status_code_int_val = 200;
        r.reason_phrase = "OK";
        r.addHeader("Content-Length", to_string(r.body.length()));
    }
    else
    {
        CgiHandler *cgi = Cgi(req, put_file_path);
        if (cgi) {
            if (cgi->executeCgiScript())
            {
                r.body = cgi->getResponseBody();
                r.status_code_int_val = cgi->getStatusCode();
                r.addHeader("Content-Type", cgi->getStrContntentType().substr(14));
                r.reason_phrase = cgi->getStrStatusCode();
                r.addHeader("Content-Length", to_string(cgi->getResponseBody().size()));
                r.addHeader("Set-Cookie", "<21school>=<21school>");
            }
            else
                r = ErrorResponseFactory::create(ErrorResponseFactory::PAYLOAD_TO_LARGE);
            delete cgi;
        }
        else
            r = ErrorResponseFactory::create(ErrorResponseFactory::PAYLOAD_TO_LARGE);
    }
}

Response RequestHandler::_makeDirectoryListingPageResponse(const std::string &dir_path) const {
    std::vector<std::string> dir_file_list = _makeListOfDirFiles(dir_path);
    Response resp;
    std::vector<std::string>::const_iterator it_begin;
    std::vector<std::string>::const_iterator it_end;

    resp.status_code_int_val = 200;
    resp.reason_phrase = "ok";
    it_begin = dir_file_list.cbegin();
    it_end = dir_file_list.cend();
    for (; it_begin < it_end; it_begin++) {
        std::string htmlLinkToElementOfDirContents = _makeHtmlLinkToDirContentPiece(*it_begin);
        resp.body += htmlLinkToElementOfDirContents;
    }
    resp.addHeader("Content-Length", to_string(resp.body.length()));
    return resp;
}

std::vector<std::string> RequestHandler::_makeListOfDirFiles(const std::string &dir_path) const {
    DIR *dir_stream;
    std::vector<std::string> listOfFiles;
    struct dirent *de;

    dir_stream = opendir(dir_path.c_str());
    if (dir_stream == NULL)
        return listOfFiles;
    errno = 0;
    while ((de = readdir(dir_stream)) != NULL) {
        std::string dir_content_element_name = de->d_name;
        if (de->d_type == DT_DIR)
            dir_content_element_name += "/";
        listOfFiles.push_back(std::string(dir_content_element_name));
    }
    closedir(dir_stream);
    if (errno != 0)
        listOfFiles.clear();
    return listOfFiles;
}

std::string RequestHandler::_makeHtmlLinkToDirContentPiece(std::string dir_content_piece) const {
    std::string link;

    link = "<a href=\"" + dir_content_piece + "\">" + dir_content_piece + "</a>" + "<br>";
    return link;
}

Response RequestHandler::_makeAddSlashToDirResponse(const std::string &req_target) const {
    Response resp;

    resp.status_code_int_val = 200;
    resp.reason_phrase = std::string("ok");
    resp.body = _makeHTMLPageWithRedirect(req_target);
    resp.addHeader("Content-length", to_string(resp.body.length()));
    return resp;
}

std::string RequestHandler::_makeHTMLPageWithRedirect(const std::string &new_path) const {
    std::string page;

    page += "<head>";
    page += "\n";
    page += "<meta http-equiv=\"refresh\" content=\"0; URL=";
    page += new_path + "/";
    page += "\" />";
    page += "\n";
    page += "</head>";
    page += "\n";
    page += "<body>";
    page += "\n";
    page += "</body>";
    return page;
}

bool RequestHandler::_isTerminatedWithSlash(const std::string &url) const {
    if (url[url.length() - 1] == '/')
        return true;
    return false;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const std::string &RequestHandler::getIp() const {
    return _serverCluster.ip;
}

int RequestHandler::getPort() const {
    return _serverCluster.port;
}

const ListenerConfig &RequestHandler::getServerConfig() const {
    return _serverConfig;
}

void RequestHandler::_handleDeleteRequest(Response &response, Request &request) {
    (void) response;
    (void) request;
    std::ifstream iff;
    iff.open(request.getRequestTarget().c_str());
    iff.close();
    if (!iff) {
        response = ErrorResponseFactory::create(ErrorResponseFactory::NOT_FOUND, _serverConfig.getErrorPageDir());
        return;
    }
    else {
        remove(request.getRequestTarget().c_str());
        response.status_code_int_val = 200;
        response.reason_phrase = "OK";
        response.body = "<html>\n"
                        "  <body>\n"
                        "    <h1>File deleted.</h1>\n"
                        "  </body>\n"
                        "</html>";
        char mbstr[100];
        response.addHeader("Date", getTime(mbstr, "%a, %d %b %Y %H:%M:%S GMT", request.getRequestTarget().c_str(), false));
    }
}

/* ************************************************************************** */
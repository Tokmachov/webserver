
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "../ServerSettings/ListenerConfig.hpp"
#include "../Response/Response.hpp"
#include "../parseHeader/Request.hpp"
#include "../ServerSettings/ListenerConfig.hpp" // METHOD enum
#include "../ServerCluster/ServerCluster.hpp"
#include "../ServerConsts/ServerConsts.hpp"
#include "../../utils/utils.hpp" //t_client_addr
#include "../Response/ErrorResponseFactory.hpp"
class CgiHandler;
#include "../CgiHandler/CgiTester.hpp"
#include "../CgiHandler/CgiPhp.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <algorithm>
#include <time.h>
#include <sys/types.h> // opendir or readdir (directory listing)
#include <sys/stat.h>  // mkdir
#include <dirent.h>	   //readdir + opendir (directory listing)
#include <cstring>
#include "PathComponents.hpp"
class RequestHandler
{
	
	public:
		RequestHandler();
		RequestHandler(const ServerCluster &serverCluster, t_client_addr addr);
		RequestHandler( RequestHandler const & src );
		~RequestHandler();
		
		RequestHandler &		operator=( RequestHandler const & rhs );
		
		//methods
		Response handleRequest(Request &req);
		//Accessors
		const std::string &getIp() const;
		int getPort() const;
		const ListenerConfig &getServerConfig() const;
	private:
		ServerCluster _serverCluster;
		ListenerConfig _serverConfig;
		t_client_addr _client_addr;
		t_location	_location;
		PathComponents _pathComponents;
		bool _isRequestParsedCorrectly(Response &resp, const Request &req) const;
		bool _isHostHeaderPresent(Response &r, const Request &req) const;
		ListenerConfig _choseServerConfig(const std::string &host) const;
		bool _choseLocation(t_location *location, const std::string &req_target) const;
		bool _isSubpath(std::string subpath, const std::string &path) const;
		
		bool _isStatusLineValid(Response &r, const Request &req) const;
		bool _isMethodValidForServer(const Request &req) const;
		bool _isMethodImplementedForLocation(const Request &req) const; 
		bool _isProtocolValid(const Request &req) const;
		
		
		void _handleGetRequest(Response &r, const Request &req);
		void _handleHeadRequest(Response &r, const Request &req); 
		
		CgiHandler *Cgi(const Request &request, const std::string &put_file_path);
		void _handlePutRequest(Response &r, const Request &req);
		void _openOrCreateDestFile(int *result, int *fd, std::string path) const;
		Response _makePutRequestResponse
		(
			int open_file_result, 
			std::string file_path
		) const;
		std::string _createLastModifiedHeaderValue(const std::string &file_path) const;

		void _handlePostRequest(Response &r, const Request &req);
		std::string	_getPagePath(const Request &req);

		std::vector<std::string> _makeListOfDirFiles(const std::string &dir_path) const;
		Response _makeDirectoryListingPageResponse(const std::string &dir_path) const;
		std::string _makeHtmlLinkToDirContentPiece(std::string dir_content_piece) const;
		Response _makeAddSlashToDirResponse(const std::string &req_target) const;
		std::string _makeHTMLPageWithRedirect(const std::string &new_path) const;
		bool _isTerminatedWithSlash(const std::string &url) const;

        void _handleDeleteRequest(Response &response, Request &request);
};

#endif /* ************************************************** REQUESTHANDLER_H */
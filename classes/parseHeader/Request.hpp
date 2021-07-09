#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <vector>
# include <map>
# include "../ServerSettings/ListenerConfig.hpp"

class Request
{
private:
	std::vector<std::string>			_allLinesRequest;
	std::vector<std::string>			_startLine;
	std::string							_versionProtocol;
	std::string							_requestTarget;
	std::string							_methodStr;
	std::string							_remainder;
	std::string							_body;
	bool								_bodyFlag;
	std::map<std::string, std::string>	_headers;
	size_t								_contentLength;
	bool								_flagTE;
	size_t								_TELength;
	std::string							_tempBody;
	bool								_isOK;
	Method								_methodEnum;
	std::string							_queryString;
	bool								_isBadRequest;
	std::string							_userAndPass;
	std::string							_user;
	std::string							_pass;

public:
	Request();
	~Request();

	std::string					setBody(std::string req);
	void						setAllLinesRequest(std::string fullRequest);
	void						setRemainder(std::string fullRequest);
	void						setStartLine();
	void						setHeaders();
	std::string					setFullRequest(std::string req);
	void						addBody(std::string req);
	void						checkBody();
	void						clear();
	std::string					ft_decodeBase64(std::string auth);
	std::string					ft_encodeBase64(std::string lp);
	Request						*syntax(std::string req);
	std::vector<std::string>	getAllLineRequest() const;
	std::vector<std::string>	getStartLine() const;
	void						printHeaders() const;
	std::string					getRemainder() const;
	std::string					getMethodStr() const;
	Method						getMethodEnum() const;
	std::string					getRequestTarget() const;
	std::string					getVersionProtocol() const;
	std::string					getBody() const;
	bool						getBodyFlag() const;
	const std::string			getValueHeader(std::string key) const;
	bool						getIsOK() const;
	const std::map<std::string, std::string>	&getHeaders() const;
	std::string					getQueryString() const;
	void						setRequestTarget(std::string rt);
	bool						getIsBadRequest() const;
	int							getContenthLength() const;
	void						setIsOK(bool set);
	void						setBodyFlag(bool set);
	std::string					getUserAndPass() const;
	std::string					getUser() const;
	std::string					getPass() const;
};

std::ostream &			operator<<( std::ostream & o, Request const & i );

#endif

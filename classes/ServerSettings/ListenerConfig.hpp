
#ifndef LISTENERCONFIG_HPP
#define LISTENERCONFIG_HPP

#include <iostream>
#include <string>
#include <vector>



enum Method
{
	GET,
	PUT,
	POST,
	HEAD,
	DELETE,
	UNSUPPORTED
};

std::string getMethodStr(Method m);

typedef struct s_location
{
	std::string location;
	std::string index;
	std::string autoindex;
	std::string allowMethods;
	std::vector<Method> methods;
	std::string root;
	std::string auth;
	std::string exec;
	size_t 		maxBody;
} t_location;

class ListenerConfig

{
private:
	unsigned int _port;
	std::string _ip;
	std::string _ipAndPort;
	unsigned int _max_waiting_connection_requests;
	std::string _serverName;
	std::string _errorPage;
	std::vector<t_location> _vectorLocations;

public:
	ListenerConfig();
	ListenerConfig(ListenerConfig const &src);
	~ListenerConfig();

	ListenerConfig &operator=(ListenerConfig const &rhs);

	const unsigned int &getPort() const;
	const std::string &getIp() const;
	const std::string &getIpAndPort() const;
	const unsigned int &getMaxWaitingConnectionRequests() const;
	const std::string &getServerName() const;
	const std::string &getErrorPageDir() const;
	const std::vector<t_location> &getVectorLocation() const;

	void setPort(const unsigned int &port);
	void setIp(const std::string &ip);
	void setIpAndPort(const std::string &ipAndPort);
	void setMaxWaitingConnectionRequests(const unsigned int &max_waiting_connection_requests);
	void setSeverName(const std::string &serverName);
	void setErrorPage(const std::string &errorPage);
	void setVectorLocation(const std::vector<t_location> &vectorlocation);
};

std::ostream &operator<<(std::ostream &o, ListenerConfig const &i);
#endif /* *********************************************** CONNECTIONSCONFIG_H */
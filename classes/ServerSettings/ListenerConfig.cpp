// /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenerConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/01 11:55:45 by sjolynn           #+#    #+#             */
/*   Updated: 2021/03/09 14:53:49 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListenerConfig.hpp"

#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define END "\x1b[0m"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ListenerConfig::ListenerConfig() : _port(9999),
								   _ip("0.0.0.0"),
								   _max_waiting_connection_requests(10),
								   _serverName("TestServer")
{
	
}

ListenerConfig::ListenerConfig(const ListenerConfig &src)
{
	*this = src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ListenerConfig::~ListenerConfig()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ListenerConfig &ListenerConfig::operator=(ListenerConfig const &rhs)
{
	if (this != &rhs)
	{
		_port = rhs.getPort();
		_ip = rhs.getIp();
		_ipAndPort = rhs.getIpAndPort();
		_max_waiting_connection_requests = rhs.getMaxWaitingConnectionRequests();
		_serverName = rhs.getServerName();
		_errorPage = rhs.getErrorPageDir();
		_vectorLocations = rhs.getVectorLocation();
	}
	return *this;
}

std::ostream &operator<<(std::ostream &o, ListenerConfig const &i)
{

	o << GREEN << "ListenerConfig :\n"
	  << END
	  << "_port = " << i.getPort()
	  << "\n_ip = " << i.getIp()
	  << "\n_ipAndPort = " << i.getIpAndPort()
	  << "\n_max_waiting_connection_requests = " << i.getMaxWaitingConnectionRequests()
	  << "\n_serverName = " << i.getServerName()
	  << "\n_errorPage = " << i.getErrorPageDir()
	  << "\n";
	unsigned int index = 0;
	for (std::vector<t_location>::const_iterator itBegin = i.getVectorLocation().begin(); itBegin != i.getVectorLocation().end(); itBegin++, index++)
	{
		o << YELLOW << "t_location: " << index << "\n"
		  << END
		  << "location: " << (*itBegin).location << "\n"
		  << "index: " << (*itBegin).index << "\n"
		  << "autoindex: " << (*itBegin).autoindex << "\n"
		  << "allowMethods: " << (*itBegin).allowMethods << "\n"
		  << "root: " << (*itBegin).root << "\n"
		  << "exec: " << (*itBegin).exec << "\n"
		  << "max body: " << (*itBegin).maxBody << "\n"
		  << "METHOD ENUM: ";
		for (std::vector<Method>::const_iterator itBeginInMethod = (*itBegin).methods.begin(); itBeginInMethod != (*itBegin).methods.end(); itBeginInMethod++)
			o << *itBeginInMethod << " ";
		o << "\n";
	}
	return o;
}

/*
** --------------------------------- GLOBAL FUNCIONS ---------------------------
*/

std::string getMethodStr(Method m)
{
	switch (m)
	{
	case GET:
		return "GET";
	case HEAD:
		return "HEAD";
	case PUT:
		return "PUT";
	case POST:
		return "POST";
    case DELETE:
         return "POST";
	case UNSUPPORTED:
		return "UNSUPPORTED";
	}
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const unsigned int &ListenerConfig::getPort() const
{
	return _port;
}

const std::string &ListenerConfig::getIp() const
{
	return _ip;
}

const std::string &ListenerConfig::getIpAndPort() const
{
	return _ipAndPort;
}

const unsigned int &ListenerConfig::getMaxWaitingConnectionRequests() const
{
	return _max_waiting_connection_requests;
}

const std::string &ListenerConfig::getServerName() const
{
	return _serverName;
}

const std::string &ListenerConfig::getErrorPageDir() const
{
	return _errorPage;
}

const std::vector<t_location> &ListenerConfig::getVectorLocation() const
{
	return _vectorLocations;
}

void ListenerConfig::setPort(const unsigned int &port)
{
	_port = port;
}

void ListenerConfig::setIp(const std::string &ip)
{
	_ip = ip;
}

void ListenerConfig::setIpAndPort(const std::string &ipAndPort)
{
	_ipAndPort = ipAndPort;
}

void ListenerConfig::setMaxWaitingConnectionRequests(const unsigned int &max_waiting_connection_requests)
{
	_max_waiting_connection_requests = max_waiting_connection_requests;
}

void ListenerConfig::setSeverName(const std::string &serverName)
{
	_serverName = serverName;
}
void ListenerConfig::setErrorPage(const std::string &errorPage)
{
	_errorPage = errorPage;
}

void ListenerConfig::setVectorLocation(const std::vector<t_location> &vectorlocation)
{
	_vectorLocations = vectorlocation;
}

/* ************************************************************************** */
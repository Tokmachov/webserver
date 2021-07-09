/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCluster.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjolynn <sjolynn@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/15 20:37:07 by sjolynn           #+#    #+#             */
/*   Updated: 2021/03/15 20:43:58 by sjolynn          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerCluster.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerCluster::ServerCluster()
{
}

ServerCluster::ServerCluster( const ServerCluster & src )
{
	ip = src.ip;
	port = src.port;
	_configs = src._configs;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ServerCluster::~ServerCluster()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ServerCluster &				ServerCluster::operator=( ServerCluster const & rhs )
{
	if ( this != &rhs )
	{
		ip = rhs.ip;
		port = rhs.port;
		_configs = rhs._configs;
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, ServerCluster const & i )
{
	o << "Server" << std::endl;
	o << "Port: " << i.port << std::endl;
	o << "IP: " << i.ip << std::endl;
	std::vector<ListenerConfig>::const_iterator it;
	for (it = i._configs.begin(); it < i._configs.end(); it++)
	{
		o << "---Config---" << std::endl;
		o << "ip: " << it->getIp() << std::endl;
		o << "port: " << it->getPort() << std::endl;
		o << "server name: " << it->getServerName() << std::endl;
	}
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/
void ServerCluster::addConfig(ListenerConfig cfg)
{
	_configs.push_back(cfg);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */
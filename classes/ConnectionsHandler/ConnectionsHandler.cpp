
#include "ConnectionsHandler.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ConnectionsHandler::ConnectionsHandler(std::vector<ListenerConfig> configs)
{
	_makeServerClusters(configs);
	_printClusters();
}

ConnectionsHandler::ConnectionsHandler(const ConnectionsHandler &src)
	: _server_clusters(src._server_clusters)
{
}

ConnectionsHandler &ConnectionsHandler::operator=(const ConnectionsHandler &rhs)
{
	if (this != &rhs)
	{
		this->_server_clusters = rhs._server_clusters;
	}
	return *this;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ConnectionsHandler::~ConnectionsHandler()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/*
** --------------------------------- METHODS ----------------------------------
*/

void ConnectionsHandler::startServer()
{
	_createListenSockets();
	_bindAddressesToListenSockets();
	_markSocketsAsListening();
	_startHandlingEvents();
}

/*
** --------------------------------- PRIVATE METHODS ---------------------------
*/

//-----------------------------------------------------------------------------//
void ConnectionsHandler::_makeServerClusters(std::vector<ListenerConfig> configs)
{
	ServerCluster cluster;
	if (configs.empty())
		return;
	while (configs.empty() == false)
	{
		cluster.ip = configs[0].getIp();
		cluster.port = configs[0].getPort();
		_addConfigsToCluster(cluster, configs, cluster.ip, cluster.port);
		_server_clusters.push_back(cluster);
		_removeCfgsAddedToCluster(configs, cluster.ip, cluster.port);
		cluster = ServerCluster();
	}
}

void ConnectionsHandler::_addConfigsToCluster(
	ServerCluster &cluster,
	const std::vector<ListenerConfig> &cfgs,
	std::string ipToAdd,
	unsigned int portToAdd)
{
	std::vector<ListenerConfig>::const_iterator it;
	for (it = cfgs.begin(); it < cfgs.end(); it++)
	{
		if (it->getPort() == portToAdd && it->getIp() == ipToAdd)
		{
			cluster.addConfig(*it);
		}
	}
}

void ConnectionsHandler::_removeCfgsAddedToCluster(
	std::vector<ListenerConfig> &configs,
	std::string ipToRemove,
	unsigned int portToRemove)
{
	std::vector<ListenerConfig>::iterator it;
	it = configs.begin();
	while (it < configs.end())
	{
		if (it->getIp() == ipToRemove && it->getPort() == portToRemove)
			configs.erase(it);
		else
			it++;
	}
}

//-----------------------------------------------------------------------------//
void ConnectionsHandler::_printClusters()
{
	std::vector<ServerCluster>::iterator it;
	for (it = _server_clusters.begin(); it < _server_clusters.end(); it++)
	{
		std::cout << *it << std::endl;
	}
}
//-----------------------------------------------------------------------------//
void ConnectionsHandler::_createListenSockets()
{
	std::vector<ServerCluster>::iterator it;
	int _socket_fd;

	for (it = _server_clusters.begin(); it < _server_clusters.end(); it++)
	{
		_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_socket_fd < 0)
			throw ConnectionHandlerException("Error: failed to create socket");
		_ls_fds_and_clusters.insert(std::pair<int, ServerCluster>(_socket_fd, *it));
	}
}
//-----------------------------------------------------------------------------//
void ConnectionsHandler::_bindAddressesToListenSockets()
{
	std::map<int, ServerCluster>::iterator it;
	sockaddr_in lsn_sock_addr;
	int bind_result;
	int opt = 1;
	for (it = _ls_fds_and_clusters.begin(); it != _ls_fds_and_clusters.end(); it++)
	{
		_createListenSockAddr(&lsn_sock_addr, it->second.ip, it->second.port);
		setsockopt(it->first, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		bind_result = bind(it->first, (sockaddr *)&lsn_sock_addr, sizeof(lsn_sock_addr));
		if (bind_result != 0)
			throw ConnectionHandlerException("Error: failed to bind address to listen socket");
	}
}

void ConnectionsHandler::_createListenSockAddr(sockaddr_in *addr, std::string ip, unsigned int port)
{
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(ip.c_str());
	addr->sin_port = _hostToNetworkShort(port);
}
//-----------------------------------------------------------------------------//
void ConnectionsHandler::_markSocketsAsListening()
{
	int result;

	std::map<int, ServerCluster>::iterator it;

	for (it = _ls_fds_and_clusters.begin(); it != _ls_fds_and_clusters.end(); it++)
	{
		result = listen(it->first, ServerConsts::maxWaitingRequests);
		if (result != 0)
			throw ConnectionHandlerException("Failed to mark socket as listening");
	}
}
//-----------------------------------------------------------------------------//
#include <errno.h>
#include <string.h>

void ConnectionsHandler::_startHandlingEvents()
{
	fd_set read_fds;
	fd_set write_fds;
	int max_d;
	int select_res;
	struct timeval time_out;

	time_out = _getTimeOutValue();
	int countSelect = 0;
	while (true)
	{
		_resetFdSets(&read_fds, &write_fds);
		max_d = -1;
		_startTrackingChangeInListenSockets(&read_fds, &max_d);
		_startTrackingChangesInSessionsSockets(&read_fds, &write_fds, &max_d);
		std::cout << "current countSelect: " << countSelect << '\n';
		select_res = select(max_d + 1, &read_fds, &write_fds, NULL, &time_out);
		std::cout << "next countSelect: " << ++countSelect << "\n\n";
		if (select_res < 0)
		{
			continue;
		}
		_clearExpiredSessions();
		if (_didReceiveConnectionsRequests(&read_fds))
			_createSessionsForIncomingConnections(&read_fds);
		_launchReadingSessionsWithChanges(&read_fds);
		_launchWritingSessionsWithChanges(&write_fds);
		_removeClosedSessions();
	}
}

void ConnectionsHandler::_resetFdSets(fd_set *r_set, fd_set *wr_set)
{
	FD_ZERO(r_set);
	FD_ZERO(wr_set);
}

struct timeval ConnectionsHandler::_getTimeOutValue() const
{
	struct timeval time;

	time.tv_sec = ServerConsts::sessionTimeOutInSeconds;
	time.tv_usec = 0;
	return time;
}

void ConnectionsHandler::_startTrackingChangeInListenSockets(fd_set *r_set, int *max_d)
{
	std::map<int, ServerCluster>::iterator it;
	int _ls_fd;

	for (it = _ls_fds_and_clusters.begin(); it != _ls_fds_and_clusters.end(); it++)
	{
		_ls_fd = it->first;
		FD_SET(_ls_fd, r_set);
		*max_d = _ls_fd;
	}
}

void ConnectionsHandler::_startTrackingChangesInSessionsSockets(fd_set *r_set, fd_set *wr_set, int *max_d)
{
	std::map<int, Session>::iterator it;
	int fd;
	for (it = _fds_and_sessions.begin(); it != _fds_and_sessions.end(); it++)
	{
		if ((*it).second.getState() == Session::READING)
		{
			fd = (*it).second.getSessionFd();
			FD_SET(fd, r_set);
			if (fd > *max_d)
				*max_d = fd;
		}
		else if ((*it).second.getState() == Session::WRITING)
		{
			fd = (*it).second.getSessionFd();
			FD_SET(fd, wr_set);
			if (fd > *max_d)
				*max_d = fd;
		}
	}
}

bool ConnectionsHandler::_didReceiveConnectionsRequests(fd_set *r_set)
{
	std::map<int, ServerCluster>::iterator it;

	for (it = _ls_fds_and_clusters.begin(); it != _ls_fds_and_clusters.end(); it++)
	{
		if (FD_ISSET(it->first, r_set))
			return true;
	}
	return false;
}

void ConnectionsHandler::_createSessionsForIncomingConnections(fd_set *r_set)
{
	socklen_t client_addr_len;
	std::map<int, ServerCluster>::iterator it_begin;
	int ls_fd;
	int client_fd;
	sockaddr_in client_addr;
	t_client_addr client_addr_to_store;
	Session new_session;
	ServerCluster serverCluster;

	client_addr_len = sizeof(client_addr);
	for (it_begin = _ls_fds_and_clusters.begin(); it_begin != _ls_fds_and_clusters.end(); it_begin++)
	{
		ls_fd = it_begin->first;
		if (FD_ISSET(ls_fd, r_set))
		{
			client_fd = accept(ls_fd, (sockaddr *)&client_addr, &client_addr_len);
			serverCluster = it_begin->second;
			client_addr_to_store = _getClientAddr(&client_addr);
			if (client_fd >= 0)
			{
				new_session = Session(client_fd, serverCluster, client_addr_to_store);
				_addNewSession(new_session);
			}
		}
	}
}

t_client_addr ConnectionsHandler::_getClientAddr(sockaddr_in *client_sock_addr)
{
	t_client_addr client_addr;

	client_addr.port = ntohs(client_sock_addr->sin_port);
	client_addr.ip = inet_ntoa(client_sock_addr->sin_addr);
	return client_addr;
}

void ConnectionsHandler::_addNewSession(Session s)
{
	_fds_and_sessions.insert(std::pair<int, Session>(s.getSessionFd(), s));
}

void ConnectionsHandler::_clearExpiredSessions()
{
	std::map<int, Session>::iterator it_begin;
	std::map<int, Session>::iterator it_end;
	time_t pending_time;

	it_begin = _fds_and_sessions.begin();
	it_end = _fds_and_sessions.end();
	for (; it_begin != it_end;)
	{
		pending_time = it_begin->second.getPendingTime();
		if (pending_time >= ServerConsts::sessionTimeOutInSeconds)
		{
			close(it_begin->second.getClientFd());
			_fds_and_sessions.erase(it_begin++);
		}
		else
			it_begin++;
	}
}

void ConnectionsHandler::_launchReadingSessionsWithChanges(fd_set *set)
{
	std::map<int, Session>::iterator it;
	int fd;

	for (it = _fds_and_sessions.begin(); it != _fds_and_sessions.end(); it++)
	{
		if ((*it).second.getState() == Session::READING)
		{
			fd = (*it).second.getSessionFd();
			if (FD_ISSET(fd, set))
			{
				it->second.proceed();
			}
		}
	}
}

void ConnectionsHandler::_launchWritingSessionsWithChanges(fd_set *set)
{
	std::map<int, Session>::iterator it;
	int fd;

	for (it = _fds_and_sessions.begin(); it != _fds_and_sessions.end(); it++)
	{
		if ((*it).second.getState() == Session::WRITING)
		{
			fd = (*it).second.getSessionFd();
			if (FD_ISSET(fd, set))
			{
				it->second.proceed();
			}
		}
	}
}

void ConnectionsHandler::_removeClosedSessions()
{
	std::map<int, Session>::iterator it;

	for (it = _fds_and_sessions.begin(); it != _fds_and_sessions.end();)
	{
		if (it->second.getState() == Session::CLOSED)
		{
			close(it->second.getClientFd());
			_fds_and_sessions.erase(it++);
		}
		else
			++it;
	}
}

//-----------------------------------------------------------------------------//
void ConnectionsHandler::_closeLstnSockets()
{
	std::map<int, ServerCluster>::iterator it;
	for (it = _ls_fds_and_clusters.begin(); it != _ls_fds_and_clusters.end(); it++)
	{
		close(it->first);
	}
}
//-----------------------------------------------------------------------------//
unsigned short int ConnectionsHandler::_hostToNetworkShort(unsigned short int val)
{
	if (_isCurrentOSBigEndian())
		return val;
	return  (val << 8) | (val >> 8 );
}

bool ConnectionsHandler::_isCurrentOSBigEndian()
{
	int n = 1;
	if (*(char *)&n == 1)
		return false;
	return true;
}

//-----------------------------------------------------------------------------//
/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/*
** --------------------------------- EXCEPTIONS --------------------------------
*/

ConnectionsHandler::ConnectionHandlerException::ConnectionHandlerException(std::string msg)
	: _msg(msg)
{
}

ConnectionsHandler::ConnectionHandlerException::~ConnectionHandlerException() throw()
{
}

char const *ConnectionsHandler::ConnectionHandlerException::what() const throw()
{
	return _msg.c_str();
}

/* ************************************************************************** */
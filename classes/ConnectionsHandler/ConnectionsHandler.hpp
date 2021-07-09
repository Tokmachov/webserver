#ifndef CONNECTIONSHANDLER_HPP
# define CONNECTIONSHANDLER_HPP

# include <iostream>
# include <string>
# include <map> //keep sessions
# include <vector> // keep read_fds write_fds

# include <sys/types.h> //
# include <sys/socket.h> //for socket AF communication type
# include <netinet/in.h> //sockadr_in  + для inet_addr + htons
# include <arpa/inet.h> // inet_addr
# include <unistd.h> // close
# include <sys/select.h>  //select

# include "../ServerSettings/ListenerConfig.hpp"
# include "../Session/Session.hpp"
# include "../RequestHandler/RequestHandler.hpp"
# include "../ServerCluster/ServerCluster.hpp"
# include "../ServerConsts/ServerConsts.hpp"
# include "../../utils/utils.hpp" //t_clinet_addr
# include "sys/time.h" // select param - time out

class ConnectionsHandler
{

	public:

		ConnectionsHandler(std::vector<ListenerConfig> configs);
		ConnectionsHandler(const ConnectionsHandler &src);

		ConnectionsHandler &operator=(const ConnectionsHandler &rhs);
		
		~ConnectionsHandler();
		
		void startServer();
	
	private:
		std::vector<ServerCluster> _server_clusters;
		std::map<int, ServerCluster> _ls_fds_and_clusters;
		std::map<int, Session> _fds_and_sessions;
		
		//functions
		void _makeServerClusters(std::vector<ListenerConfig> configs);
		void _removeCfgsAddedToCluster
		(
			std::vector<ListenerConfig> &configs, 
			std::string ipToRemove, 
			unsigned int portToRemove
		);
		void _addConfigsToCluster
		(
			ServerCluster &cluster,
			const std::vector<ListenerConfig> &cfgs,
			std::string ipToAdd, 
			unsigned int portToAdd
		);

		void _createListenSockets();
		
		void _bindAddressesToListenSockets();
		void _createListenSockAddr(sockaddr_in *addr, std::string ip, unsigned int port);		
		
		void _markSocketsAsListening();
		
		void _printClusters();

		void _startHandlingEvents();
		void _resetFdSets(fd_set *r_set, fd_set *wr_set);
		struct timeval _getTimeOutValue() const;
		void _startTrackingChangeInListenSockets(fd_set *r_set, int *max_d);
		void _startTrackingChangesInSessionsSockets(fd_set *r_set, fd_set *wr_set, int *max_d);
		bool _didReceiveConnectionsRequests(fd_set *r_set);
		
		void _createSessionsForIncomingConnections(fd_set *r_set);
		t_client_addr _getClientAddr(sockaddr_in *client_sock_addr);
		void _addNewSession(Session s);
		
		void _clearExpiredSessions();
		void _launchReadingSessionsWithChanges(fd_set *set);
		void _launchWritingSessionsWithChanges(fd_set *set);
		void _removeClosedSessions();
		
		void _closeLstnSockets();
		
		unsigned short int _hostToNetworkShort(unsigned short int val);
		bool _isCurrentOSBigEndian();
	class ConnectionHandlerException: public std::exception
	{
		private:
			std::string _msg;
		public:
			ConnectionHandlerException(std::string msg);
			~ConnectionHandlerException() throw();
			virtual char const *what() const throw();
	};

};

#endif /* ********************************************** CONNECTIONSHANDLER_H */
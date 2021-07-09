/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/01 15:12:56 by sjolynn           #+#    #+#             */
/*   Updated: 2021/04/12 11:51:46 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
# define SESSION_HPP

# include <iostream>
# include <string>

# include <vector>
# include <unistd.h> //fcntl writing
# include <fcntl.h> //fcntl writing
# include <stdlib.h>

# include "../../utils/utils.hpp" //t_client addr
# include "../RequestHandler/RequestHandler.hpp"
# include "../parseHeader/Request.hpp"
# include "../ServerCluster/ServerCluster.hpp"
# include <sys/time.h> // for _pendingStart attr

class Session
{
	# define READING_SUCCESS 1	
	# define READING_ERROR -1
	# define READING_NEED_MORE 0
	public:
		Session();
		Session(int client_fd, const ServerCluster &serverCluster, t_client_addr cl_addr);
		
		Session(const Session &src);
		Session &operator=(const Session &rhs);
		
		virtual ~Session();
		//types
		enum State
		{
			READING = 1,
			WRITING = 2,
			CLOSED = 3
		};
		//Accessors
		int getSessionFd() const;
		State getState() const;
		const std::string &getIp() const;
		int getPort() const;
		int getClientFd();
		time_t getPendingTime() const;
		//functions
		void proceed();
				
	private:
		int	_client_fd;
		State	_state;
		Request	_request;
		RequestHandler	_requestHandler;
		std::string	_response_raw;
		struct timeval	_pendingStart;
		t_client_addr _client_addr;
		std::multimap<std::string , Response> _sessionMeneger;
		//functions
		void _setNonBlockingMode(int _fd);
		void _resetPendingStart();
		
		//static members
		static const int _readBuffSize;
};

#endif /* ********************************************************* SESSION_H */
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/01 15:12:56 by sjolynn           #+#    #+#             */
/*   Updated: 2021/04/26 15:12:44 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
#include "Session.hpp"
#include <sys/socket.h>
#include <sys/types.h>
Session::Session()
{
}

Session::Session(int client_fd, const ServerCluster &serverCluster, t_client_addr cl_addr)
	: _client_fd(client_fd), _state(READING), _requestHandler(serverCluster, cl_addr), _client_addr(cl_addr)
{
	gettimeofday(&_pendingStart, NULL);
}

Session::Session(const Session &src)
{
	_client_fd = src._client_fd;
	_state = src._state;
	_request = src._request;
	_requestHandler = src._requestHandler;
	_response_raw = src._response_raw;
	_pendingStart = src._pendingStart;
	_client_addr = src._client_addr;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Session::~Session()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/
Session &Session::operator=(const Session &rhs)
{
	if (this != &rhs)
	{
		_client_fd = rhs._client_fd;
		_state = rhs._state;
		_request = rhs._request;
		_requestHandler = rhs._requestHandler;
		_response_raw = rhs._response_raw;
		_pendingStart = rhs._pendingStart;
		_client_addr = rhs._client_addr;
	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/
void Session::proceed()
{
	int rw_result;
	char buf[_readBuffSize + 1];
	Response response;

	switch (_state)
	{
	case READING:
		memset(buf, '\0', _readBuffSize + 1);
		rw_result = read(_client_fd, buf, _readBuffSize);
		_resetPendingStart();
		if (rw_result > 0)
		{
			this->_request.syntax(buf);
			if (this->_request.getIsOK())
			{
                if (!_sessionMeneger.empty() && (_request.getMethodEnum() == GET || _request.getMethodEnum() == HEAD)) {
                    std::pair<std::multimap<std::string , Response>::iterator, std::multimap<std::string , Response>::iterator> ret;
                    ret = _sessionMeneger.equal_range(std::string(_client_addr.ip));
                    for (std::multimap<std::string , Response>::iterator it=ret.first; it!=ret.second; ++it) {
                        if (it->second.target == _request.getRequestTarget() && it->second.method == _request.getMethodEnum())
                        {
                            _response_raw = it->second.getString();
                            _state = WRITING;
                            break;
                        }
                    }
                    response = _requestHandler.handleRequest(this->_request);
                    _response_raw = response.getString();
                    if (_request.getMethodEnum() == GET || _request.getMethodEnum() == HEAD)
                        _sessionMeneger.insert(std::make_pair(std::string(_client_addr.ip),response));
                    _state = WRITING;
                }
                else {
                    response = _requestHandler.handleRequest(this->_request);
                    _response_raw = response.getString();
                    if (_request.getMethodEnum() == GET || _request.getMethodEnum() == HEAD)
                        _sessionMeneger.insert(std::make_pair(std::string(_client_addr.ip),response));
                    _state = WRITING;
                }
			}
		}
		else
			_state = CLOSED;
		break;
	case WRITING:
		_setNonBlockingMode(_client_fd);
		rw_result = send(this->_client_fd, _response_raw.c_str(), _response_raw.length(), 0);
		_resetPendingStart();
		if (rw_result == (long long)((_response_raw.length())))
		{
			if (this->_request.getValueHeader("connection") == "close")
			{
				_state = CLOSED;
			}
			else
			{
				_response_raw.clear();
				_request.clear();
				_state = READING;
			}
		}
		else if (rw_result > 0 && rw_result < (long long)(_response_raw.length()))
		{
			_response_raw.replace(0, rw_result, "");
			_state = WRITING;
		}
		else
		{
			_state = CLOSED;
		}
		break;
	case CLOSED:
		break;
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int Session::getSessionFd() const
{
	return _client_fd;
}

Session::State Session::getState() const
{
	return _state;
}

const std::string &Session::getIp() const
{
	return _requestHandler.getIp();
}

int Session::getPort() const
{
	return _requestHandler.getPort();
}

int Session::getClientFd()
{
	return _client_fd;
}

time_t Session::getPendingTime() const
{
	struct timeval current_time;

	gettimeofday(&current_time, NULL);
	return current_time.tv_sec - _pendingStart.tv_sec;
}

/*
** --------------------------------- PRIVATE METHODS ---------------------------
*/

//----------------------------_setNonBlockingMode()-----------------------------

void Session::_setNonBlockingMode(int _fd)
{
	int flags = fcntl(_fd, F_GETFL);
	fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
}

void Session::_resetPendingStart()
{
	gettimeofday(&_pendingStart, NULL);
}

/*
** --------------------------------- STATIC VARS---------------------------------
*/

const int Session::_readBuffSize = 4095;
/* ************************************************************************** */
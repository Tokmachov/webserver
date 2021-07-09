/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/22 15:40:24 by anatashi          #+#    #+#             */
/*   Updated: 2021/04/19 17:50:39 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#include "../parseHeader/Request.hpp"
#include "../../utils/utils.hpp"


class CgiHandler
{

private:

protected:
	std::string _requestBody;
	std::string _responseBody;
	char **_env;
	const std::string *const _putFilePath;
	const ListenerConfig *const _serverConfig;
	const t_location *const _location;
	int _fd;
	
	int _statusCode;
	std::string _strStatusCode;
	std::string _strContentType;

	CgiHandler();
	CgiHandler(const CgiHandler &copy);
	CgiHandler &operator=(CgiHandler const &rhs);
	
	void freeMemory(char **array);
	virtual void parseCGI();
	virtual const std::string *getFile(const Request *const request, const std::string *const putFilePath);
    virtual const std::string *getFile(const Request *const request, const t_location *const location);
public:
	CgiHandler(const Request *const request,
			   const ListenerConfig *const serverConfig,
			   const t_location *const location,
			   const std::string *const putFilePath,
			   const t_client_addr *const client_addr);
	virtual ~CgiHandler();

	void createTwoDimensionalArrayEnv(const Request *const requset, const t_client_addr *const client_addr);

	virtual bool executeCgiScript();


	const std::string &getRequestBody() const;
	const std::string &getResponseBody() const;
	const char *const *getEnv() const;
	const ListenerConfig *getListenerConfig() const;
	const int &getStatusCode() const;
	const std::string &getStrStatusCode() const;
	const std::string &getStrContntentType() const;


	struct FileOpeningErrorException : std::exception
	{
		const char *what() const throw();
	};
};

#endif /* ****************************************************** CGIHANDLER_H */
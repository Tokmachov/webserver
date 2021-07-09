/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjolynn <sjolynn@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:18:48 by sjolynn           #+#    #+#             */
/*   Updated: 2021/03/24 15:49:09 by sjolynn          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <map>
# include "../../utils/utils.hpp"
# include <stdlib.h> // free itoa results
#include "ListenerConfig.hpp"

struct Response
{
	#define SP " "
	#define DELIMITER "\r\n"
	Response() : httpVersion("HTTP/1.1") {}
	
	//startline
	//HTTP-name "/" DIGIT "." DIGIT
	const std::string httpVersion;
    std::string target;
	int status_code_int_val;
	std::string reason_phrase;
	Method method;
	
	//headers
	std::map<std::string, std::string> headers;
	
	//body
	std::string body;
	
	void addHeader(std::string key, std::string value)
	{
		headers.insert( std::pair<std::string, std::string> (key, value));	
	}
	
	std::string getString()
	{
		return _getStatusLineStr() + DELIMITER + _getHeadersStr() + DELIMITER + body;
	}
	
	Response &operator=(const Response &rhs)
	{
		status_code_int_val = rhs.status_code_int_val;
		reason_phrase = rhs.reason_phrase;
		headers = rhs.headers;
		body = rhs.body;
		target = rhs.target;
		method = rhs.method;
		return *this;
	}
	
	private:
	std::string _getStatusLineStr()
	{
		std::string status_code_int_val_str;

		status_code_int_val_str = _getStatusCodeIntValAsString();
		return httpVersion + SP + status_code_int_val_str + SP + reason_phrase;
	}
		
	std::string _getStatusCodeIntValAsString()
	{
		std::string status_code_int_val_str;	
		char *status_code_int_val_c_str;

		status_code_int_val_c_str = _ft_itoa(status_code_int_val);
		status_code_int_val_str = std::string(status_code_int_val_c_str);
		free(status_code_int_val_c_str);
		return status_code_int_val_str;
	}
	std::string _getHeadersStr() 
	{
		std::string concatinated_headers;
		
		for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		{
			concatinated_headers += it->first + ":" + it->second + DELIMITER;			
		}
		return concatinated_headers;
	}
};

#endif /* ******************************************************** RESPONSE_H */
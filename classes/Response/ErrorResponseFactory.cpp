/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorResponseFactory.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/21 18:00:01 by sjolynn           #+#    #+#             */
/*   Updated: 2021/04/13 13:12:47 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorResponseFactory.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ErrorResponseFactory::ErrorResponseFactory()
{
}

ErrorResponseFactory::ErrorResponseFactory( const ErrorResponseFactory & src )
{
	(void)src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ErrorResponseFactory::~ErrorResponseFactory()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ErrorResponseFactory &				ErrorResponseFactory::operator=( ErrorResponseFactory const & rhs )
{
	(void)rhs;
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

Response ErrorResponseFactory::create
(
	const ErrorType &type, 
	const std::string &error_page_dir,
	const std::vector<Method> allowedMethods
)
{
	Response resp;
	switch (type)
	{
	case NOT_IMPLEMENTED_METHOD:
		_createMethodNotImplementedErrorResponse(resp, error_page_dir);		
		break;
	case BAD_REQUEST:
		_createBadRequestErrorResponse(resp, error_page_dir);
		break;
	case NOT_FOUND:
		_createNotFoundErrorResponse(resp, error_page_dir);
		break;
	case METHOD_NOT_ALLOWED:
		_createMethodNotAllowedErrorResponse(resp, error_page_dir, allowedMethods);
		break;
	case VERSION_NOT_SUPPORTED:
		_createVersionNotSupportedErrorResponse(resp, error_page_dir);
		break;
	case INTERNAL_SERVER_ERROR:
		_createInternalServerErrorResponse(resp, error_page_dir);
		break;
	case PAYLOAD_TO_LARGE:
		_createPayLoadToLargerErrorResponse(resp);
		break;
	}
	return resp;
}

/*
** --------------------------------- PRIVATE_METHODS ---------------------------
*/

void ErrorResponseFactory::_createMethodNotImplementedErrorResponse(Response &resp, const::std::string &error_page_dir)
{
	std::string error_page_link;
	int fd;
	int res;
	
	error_page_link = error_page_dir + ServerConsts::errorPageLocalPathForNotImplementedError_501;
	fd = open(error_page_link.c_str(), O_RDONLY);
	if (fd < 0 || (res = writeFileToBuffer(fd, &(resp.body))) != 0)
		resp.body = _formDefaultErrorPage("501 Method is not implemented");
	resp.status_code_int_val = 501;
	resp.reason_phrase = "Method is not implemented";
	resp.addHeader("content-length", to_string(resp.body.length()));
	close(fd);
}

void ErrorResponseFactory::_createBadRequestErrorResponse(Response &resp, const::std::string &error_page_dir)
{
	std::string error_page_link;
	int fd;
	int res;

	error_page_link = error_page_dir + ServerConsts::errorPageLocalPathForBadRequestError_400;
	fd = open(error_page_link.c_str(), O_RDONLY);
	if (fd < 0 || (res = writeFileToBuffer(fd, &(resp.body))) != 0)
		resp.body = _formDefaultErrorPage("400 Bad request");
	resp.status_code_int_val = 400;
	resp.reason_phrase = "Bad request";
	resp.addHeader("content-length", to_string(resp.body.length()));
	close(fd);
}

void ErrorResponseFactory::_createNotFoundErrorResponse(Response &resp, const::std::string &error_page_dir)
{
	std::string error_page_link;
	int fd;
	int res;
	
	error_page_link = error_page_dir + ServerConsts::errorPageLocalPathForNotFound_404;
	fd = open(error_page_link.c_str(), O_RDONLY);
	if (fd < 0 || (res = writeFileToBuffer(fd, &(resp.body))) != 0)
		resp.body = _formDefaultErrorPage("404 Not found");
	resp.body = _formDefaultErrorPage("Error 404 not found.");
	resp.status_code_int_val = 404;
	resp.reason_phrase = "Not found";
	resp.addHeader("content-length", to_string(resp.body.length()));
	close(fd);
}

void ErrorResponseFactory::_createMethodNotAllowedErrorResponse
(
	Response &resp, 
	const::std::string &error_page_dir, 
	std::vector<Method> allowedMethods
)
{
	std::string error_page_link;
	int fd;
	int res;
	std::string allowedMethodsHeaderValue;	
	
	error_page_link = error_page_dir + ServerConsts::errorPageLocalPathForMethodNotAllowed_405;
	fd = open(error_page_link.c_str(), O_RDONLY);
	if (fd < 0 || (res = writeFileToBuffer(fd, &(resp.body))) != 0)
		resp.body = _formDefaultErrorPage("405 Method not allowed");
	resp.status_code_int_val = 405;
	resp.reason_phrase = "Method not allowed";
	resp.addHeader("content-length", to_string(resp.body.length()));
	allowedMethodsHeaderValue = _createAllowedMethodsHeaderValue(allowedMethods);
	resp.addHeader("Allow", allowedMethodsHeaderValue);
	close(fd);
}

std::string ErrorResponseFactory::_createAllowedMethodsHeaderValue(const std::vector<Method> &allowedMethod)
{
	std::vector<Method>::const_iterator it_begin;
	std::vector<Method>::const_iterator it_end;
	std::string allowedMethods;

	it_begin = allowedMethod.cbegin();
	it_end = allowedMethod.cend();
	for (; it_begin < it_end; it_begin++)
	{
		allowedMethods += getMethodStr(*it_begin);
		if (it_begin != it_end - 1)
			allowedMethods += ",";
	}
	return allowedMethods;
}

void ErrorResponseFactory::_createVersionNotSupportedErrorResponse
(
	Response &resp,
	const::std::string &error_page_dir
)
{
	std::string error_page_link;
	int fd;
	int res;
	
	error_page_link = error_page_dir + ServerConsts::errorPageLocalPathForVersionNotSupported_505;
	fd = open(error_page_link.c_str(), O_RDONLY);
	if (fd < 0 || (res = writeFileToBuffer(fd, &(resp.body))) != 0)
		resp.body = _formDefaultErrorPage("505 Http version is not supported");
	resp.status_code_int_val = 505;
	resp.reason_phrase = "Http version is not supported";
	resp.addHeader("content-length", to_string(resp.body.length()));
	close(fd);
}

void ErrorResponseFactory::_createInternalServerErrorResponse
(
	Response &resp,
	const::std::string &error_page_dir
)
{
	std::string error_page_link;
	int fd;
	int res;
	
	error_page_link = error_page_dir + ServerConsts::errorPageLocalPathForInternalServerError_500;
	fd = open(error_page_link.c_str(), O_RDONLY);
	if (fd < 0 || (res = writeFileToBuffer(fd, &(resp.body))) != 0)
		resp.body = _formDefaultErrorPage("500 Internal server error");
	resp.status_code_int_val = 500;
	resp.reason_phrase = "Internal server error";
	resp.addHeader("content-length", to_string(resp.body.length()));
	close(fd);
}

void ErrorResponseFactory::_createPayLoadToLargerErrorResponse
(
	Response &resp
)
{
	resp.body = "";
    resp.status_code_int_val = 413;
    resp.reason_phrase = "Payload Too Large";
    resp.addHeader("Content-Length", "0");
}

std::string ErrorResponseFactory::_formDefaultErrorPage(const std::string &page_error_msg)
{
	std::string error_page = std::string("<head>");
	error_page += std::string("\n");
	error_page += std::string("<title>");
	error_page += page_error_msg;
	error_page += std::string("</title>");
	error_page += std::string("<body>");
	error_page += std::string("\n");
	error_page += std::string("<h1>");
	error_page += page_error_msg;
	error_page += std::string("</h1>");
	error_page += std::string("\n");
	error_page += std::string("</body>");
	error_page += std::string("\n");
	error_page += std::string("</head>");
	return error_page;
}

/* ************************************************************************** */
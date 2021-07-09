/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConsts.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 17:04:52 by sjolynn           #+#    #+#             */
/*   Updated: 2021/04/19 12:50:29 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConsts.hpp"
#include <sys/socket.h>
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
ServerConsts::ServerConsts() {}
ServerConsts::ServerConsts(const ServerConsts &src) { (void)src; }
ServerConsts &ServerConsts::operator=(const ServerConsts &rhs) 
{ 
    (void)rhs; 
    return *this;
}
ServerConsts::~ServerConsts() {};

int ServerConsts::maxWaitingRequests = SOMAXCONN;
int ServerConsts::sessionTimeOutInSeconds = 600;
std::vector<Method> ServerConsts::getImplementedMethods()
{
    std::vector<Method> method;
    method.push_back(GET);
    method.push_back(PUT);
    method.push_back(POST);
    method.push_back(HEAD);
    method.push_back(DELETE);
    return method;
}
std::string ServerConsts::errorPageLocalPathForNotImplementedError_501 = "/501_not_implemented/501.html";
std::string ServerConsts::errorPageLocalPathForBadRequestError_400 = "/400_bad_request/400.html";
std::string ServerConsts::errorPageLocalPathForNotFound_404 = "/404_not_found/404.html";
std::string ServerConsts::errorPageLocalPathForMethodNotAllowed_405 = "/405_method_not_allowed/405.html";
std::string ServerConsts::errorPageLocalPathForInternalServerError_500 = "/500_internal_server_error/500.html";
std::string ServerConsts::errorPageLocalPathForVersionNotSupported_505 = "/505_version_not_supported/505.html";
/* ************************************************************************** */
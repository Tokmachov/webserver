/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorResponseFactory.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjolynn <sjolynn@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/21 18:00:01 by sjolynn           #+#    #+#             */
/*   Updated: 2021/03/25 11:57:21 by sjolynn          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORRESPONSEFACTORY_HPP
# define ERRORRESPONSEFACTORY_HPP

# include <iostream>
# include <string>
# include "Response.hpp"
# include <unistd.h>
# include "../../utils/utils.hpp"
# include <exception>
#include <fcntl.h>
# include "../ServerConsts/ServerConsts.hpp"

struct ErrorResponseFactory
{
	enum ErrorType
	{
		NOT_IMPLEMENTED_METHOD,
		BAD_REQUEST,
		NOT_FOUND,
		METHOD_NOT_ALLOWED,
		VERSION_NOT_SUPPORTED,
		INTERNAL_SERVER_ERROR,
		PAYLOAD_TO_LARGE
	};
	static Response create
	(
		const ErrorType &type, 
		const std::string &error_page_dir = std::string(),
		const std::vector<Method> allowedMethods = std::vector<Method>()
	);

	private:
		ErrorResponseFactory();
		ErrorResponseFactory( ErrorResponseFactory const & src );
		~ErrorResponseFactory();

		ErrorResponseFactory &		operator=( ErrorResponseFactory const & rhs );
		
		static void _createMethodNotImplementedErrorResponse
		(
			Response &resp, 
			const::std::string &error_page_dir
		);
		static void _createBadRequestErrorResponse
		(
			Response &resp, 
			const::std::string &error_page_dir
		);
		static void _createNotFoundErrorResponse
		(
			Response &resp, 
			const::std::string &error_page_dir
		);
		static void _createMethodNotAllowedErrorResponse
		(
			Response &resp, 
			const::std::string &error_page_dir, 
			std::vector<Method> allowedMethods
		);
		static std::string _createAllowedMethodsHeaderValue
		(
			const std::vector<Method> &allowedMethod
		);
		static void _createVersionNotSupportedErrorResponse
		(
			Response &resp,
			const::std::string &error_page_dir
		);
		static void _createInternalServerErrorResponse
		(
			Response &resp,
			const::std::string &error_page_dir
		);
		static void _createPayLoadToLargerErrorResponse
		(
			Response &resp
		);
		static std::string _formDefaultErrorPage(const std::string &page_error_msg);
};

#endif /* ******************************************** ERRORRESPONSEFACTORY_H */
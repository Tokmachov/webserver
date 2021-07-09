/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiPhp.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/18 14:29:20 by anatashi          #+#    #+#             */
/*   Updated: 2021/04/19 16:31:02 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIPHP_HPP
#define CGIPHP_HPP

#include "CgiHandler.hpp"

#define PHP_PATH "/usr/bin/php"
#define PHP_NAME "php"



class CgiPhp : public CgiHandler
{ 	
	 CgiPhp();

	CgiPhp(CgiPhp const &src);
	CgiPhp &operator=(CgiPhp const &rhs);

	virtual void parseCGI();

public:
	CgiPhp(const Request *const request,
		   const ListenerConfig *const serverConfig,
		   const t_location *const location,
		   const std::string *const putFilePath,
		   const t_client_addr *const client_addr);
	virtual ~CgiPhp();

	virtual bool executeCgiScript();

private:
};

#endif /* ********************************************************** CGIPHP_H */
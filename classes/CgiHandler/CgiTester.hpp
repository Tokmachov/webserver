/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiTester.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/16 17:52:40 by anatashi          #+#    #+#             */
/*   Updated: 2021/04/18 14:13:51 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGITESTER_HPP
#define CGITESTER_HPP


#include "CgiHandler.hpp"
#include <sys/stat.h>
#include <fcntl.h>

class CgiTester : public CgiHandler
{

private:
	virtual void parseCGI();
    virtual const std::string *getFile(const Request *const request, const t_location *const location);
	std::string makePutFilePathWitNoRoot(const std::string &req_target, const t_location *const location) const;
    std::string getPutRequestFileName(std::string req_target) const;

	CgiTester();
	CgiTester(CgiTester const &src);
	CgiTester &operator=(CgiTester const &rhs);
public:
	
	CgiTester(const Request *const request,
              const ListenerConfig *const serverConfig,
              const t_location *const location,
              const t_client_addr *const client_addr);
	virtual ~CgiTester();

	virtual bool executeCgiScript();

};

#endif /* ******************************************************* CGITESTER_H */
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConsts.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjolynn <sjolynn@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 17:04:52 by sjolynn           #+#    #+#             */
/*   Updated: 2021/03/25 11:39:45 by sjolynn          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONSTS_HPP
# define SERVERCONSTS_HPP

# include <iostream>
# include <string>
# include "../ServerSettings/ListenerConfig.hpp"
# include <vector>

struct ServerConsts
{
	static int sessionTimeOutInSeconds;
	static int maxWaitingRequests;
	static std::vector<Method> getImplementedMethods();
	static std::string errorPageLocalPathForNotImplementedError_501;
	static std::string errorPageLocalPathForBadRequestError_400;
	static std::string errorPageLocalPathForNotFound_404;
	static std::string errorPageLocalPathForMethodNotAllowed_405;
	static std::string errorPageLocalPathForInternalServerError_500;
	static std::string errorPageLocalPathForVersionNotSupported_505;
	
	//Methods below must stay explicitly listed in private section
	//because it clearifies the idea that an instance of the class must never be
	//created or class must be used as type only.
	private:
		ServerConsts();
		ServerConsts(const ServerConsts &src);
		ServerConsts &operator=(const ServerConsts &rhs);
		~ServerConsts();
};

#endif /* **************************************************** SERVERCONSTS_H */
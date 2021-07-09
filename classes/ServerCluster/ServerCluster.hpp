/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCluster.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjolynn <sjolynn@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/15 20:37:07 by sjolynn           #+#    #+#             */
/*   Updated: 2021/03/15 20:51:17 by sjolynn          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCLUSTER_HPP
# define SERVERCLUSTER_HPP

# include <iostream>
# include <string>
# include "../ServerSettings/ListenerConfig.hpp"
# include <vector>

struct ServerCluster
{
		ServerCluster();
		ServerCluster( ServerCluster const & src );
		~ServerCluster();
		
		ServerCluster &		operator=( ServerCluster const & rhs );
		std::string ip;
		unsigned int port;
		std::vector<ListenerConfig> _configs;
		void addConfig(ListenerConfig cfg);

};

std::ostream &			operator<<( std::ostream & o, ServerCluster const & i );

#endif /* ***************************************************** ServerCluster_H */
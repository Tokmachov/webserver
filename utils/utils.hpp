/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/04 11:56:53 by sjolynn           #+#    #+#             */
/*   Updated: 2021/04/16 13:48:20 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUT_LOG_H
#define PUT_LOG_H

#include <string>
extern float standard_delay;
extern bool is_on;

void put_log(std::string msg, float delay_sec);
void put_log_ip_and_port(std::string msg, std::string ip, unsigned int port, float delay_sec);
void put_log_count(std::string msg, int count, float delay_sec);
void put_log_ip_port_fd(std::string msg, std::string ip, unsigned int port, int fd, float delay_sec);
int writeFileToBuffer(int fd, std::string *buff);
int _ft_isdigit(int c);
char    *_ft_itoa(int n);

/*
	s1, s2 not be NULL
*/
char *_ft_strjoin(const char *s1, const char *s2);

/*
	str not be NULL
*/
char *_ft_strdup(const char *str);

/*
	doesn't work with negative numbers
*/
char *_ft_size_tInStr(const size_t &number);
std::string to_string(int n);

typedef struct  s_client_addr
{
    uint16_t        port;
    char            *ip;
}               t_client_addr;

#endif
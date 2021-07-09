/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ft_strdup.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/16 13:25:31 by anatashi          #+#    #+#             */
/*   Updated: 2021/04/16 15:38:05 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include<cstring>

/*
	str not be NULL
*/
char *_ft_strdup(const char *str)
{
	size_t lenStr = strlen(str);
	char *newStr = new char[lenStr + 1];
	memcpy(newStr, str, lenStr);
	newStr[lenStr] = '\0';
	return newStr;
}
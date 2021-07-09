/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ft_strjoin.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anatashi <anatashi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/16 11:51:03 by anatashi          #+#    #+#             */
/*   Updated: 2021/04/16 15:38:09 by anatashi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>

/*
	s1, s2 not be NULL
*/

char *_ft_strjoin(const char *s1, const char *s2)
{
	size_t lenS1 = strlen(s1), lenS2 = strlen(s2), lenNewStr = lenS1 + lenS2;
	char *newStr = new char[lenNewStr + 1];
	memcpy(newStr, s1, lenS1);
	memcpy(newStr + lenS1, s2, lenS2);
	newStr[lenNewStr] = '\0';
	return newStr;
}
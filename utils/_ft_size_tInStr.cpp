#include<cstring>

/*
	doesn't work with negative numbers
*/

size_t __ft_lenInt(size_t number)
{
	size_t	lenNumber = 0;

	if (number == 0)
		return (1);
	while (number > 0)
	{
		number /= 10;
		lenNumber++;
	}
	return (lenNumber);	
}

void __ft_putNbrInStr(size_t number, char **buf, size_t &i)
{
	if (number >= 10)
		__ft_putNbrInStr(number / 10, buf, i);
	(*buf)[i] = number % 10 + '0';
	i++;
}

char *_ft_size_tInStr(const size_t &number)
{
	size_t lenStr = __ft_lenInt(number) + 1;
	char *str = new char[lenStr];
	size_t i = 0;
	__ft_putNbrInStr(number, &str, i);
	str[lenStr] = '\0';
	return str;
}
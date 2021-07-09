#include "utils.hpp"

static	long int	ft_intlen(long int n)
{
    int	i;
    int	sign;

    i = 0;
    if (n == 0)
        return (1);
    if ((sign = n) < 0)
    {
        n = -n;
        i++;
    }
    while (n > 0)
    {
        n /= 10;
        i++;
    }
    return (i);
}

char				*_ft_itoa(int n)
{
    char			*str;
    int				len;
    long int		nn;

    nn = (long int)n;
    len = ft_intlen(nn);
    if (!(str = (char *)malloc(len + 1)))
        return (NULL);
    str[len--] = '\0';
    if (nn < 0)
        nn = -nn;
    while (len >= 0)
    {
        str[len--] = nn % 10 + '0';
        nn /= 10;
    }
    if (n < 0)
        str[0] = '-';
    return (str);
}


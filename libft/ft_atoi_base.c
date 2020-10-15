#include "libft.h"

static int		ft_should_skip(char c)
{
    if (c == ' ' || c == '\n' || c == '\t' ||
        c == '\v' || c == '\f' || c == '\r')
        return (1);
    return (0);
}
int				ft_atoi_base(const char *str, const char *base)
{
    unsigned long	nbr;
    char            *chr;

    while (ft_should_skip(*str))
        str++;
    str += *str == '+' || *str == '-';
    nbr = 0;
    while ((chr = ft_strchr(base, *str)) && *str != 0)
    {
        nbr = nbr * ft_strlen(base) + (chr - base);
        if (nbr > LIB_LONG_MAX)
            return ((int)LIB_LONG_MAX);
        str++;
    }
    return ((int)nbr);
}

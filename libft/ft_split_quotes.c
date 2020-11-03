#include "libft.h"

static char	*ft_strnew(size_t size)
{
	return ((char *)ft_memalloc(sizeof(char) * (size + 1)));
}

static int	ft_countsplit(const char *str)
{
	int		i;
	char	quote;

	i = 0;
	while (*str)
	{
		while (*str == ' ' || *str == '\t')
			str++;
		if (*str == '\"' || *str == '\'')
		{
			quote = *str++;
			while (*str != quote)
				str++;
		}
		if (*str != '\0')
			i++;
		while (*str && !(*str == ' ' || *str == '\t'))
			str++;
	}
	return (i);
}

static int	ft_wordlen(const char *str)
{
	int		i;
	char	quote;

	i = 0;
	if (str[0] == '\"' || str[0] == '\'')
	{
		i++;
		quote = *str++;
		while (*str && *str != quote)
		{
			i++;
			str++;
		}
		i++;
	}
	else
	{
		while (*str && !(*str == ' ' || *str == '\t'))
		{
			i++;
			str++;
		}
	}
	return (i);
}

char		**ft_splitquotes(char const *str)
{
	int		count;
	int		i;
	char	**split;
	int		b;

	if (str == NULL)
		return (NULL);
	count = ft_countsplit(str);
	if (!(split = (char **)ft_memalloc((sizeof(char *) * (count + 1)))))
		return (NULL);
	i = -1;
	while (++i < count)
	{
		while (*str == ' ' || *str == '\t')
			str++;
		b = ft_wordlen(str);
		if (!(split[i] = ft_strnew(b)))
		{
			ft_freesplit(split);
			return (NULL);
		}
		ft_strlcpy(split[i], str, b);
		str += b;
	}
	return (split);
}


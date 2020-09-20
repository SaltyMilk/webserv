/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/07 10:31:12 by sel-melc          #+#    #+#             */
/*   Updated: 2019/11/05 18:35:28 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void					prep_atoi(const char *s, int *i, int *sign)
{
	*i = 0;
	*sign = 1;
	while (s[*i] == '\t' || s[*i] == '\n' || s[*i] == '\r' ||
			s[*i] == '\v' || s[*i] == '\f' || s[*i] == ' ')
		(*i)++;
	if (s[*i] == '-')
	{
		(*sign) *= -1;
		(*i)++;
	}
	else if (s[*i] == '+')
		(*i)++;
}

static	unsigned long long	ft_powa(int n, unsigned int pow)
{
	if (pow == 0)
		return (1);
	else if (pow == 1)
		return (n);
	return (n * ft_powa(n, pow - 1));
}

static	int					count_digits(const char *str, int i)
{
	int count;

	count = 0;
	while (ft_isdigit(str[i]))
	{
		count++;
		i++;
	}
	return (count);
}

int							ft_atoi(const char *str)
{
	int					i;
	int					sign;
	unsigned long long	tenpowdig;
	unsigned long long	res;
	int					digits_sstr;

	res = 0;
	prep_atoi(str, &i, &sign);
	digits_sstr = count_digits(str, i);
	tenpowdig = ft_powa(10, digits_sstr) / 10;
	while (tenpowdig >= 1)
	{
		res += (str[i] - 48) * tenpowdig;
		tenpowdig /= 10;
		i++;
		if (res > 9223372036854775807)
			return ((1 + sign) / (-2));
	}
	return ((int)res * sign);
}

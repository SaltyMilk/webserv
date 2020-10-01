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
	if (str[i] && str[i] != ' ')
		return (-1);
	return (count);
}
//STRICT ATOI ONLY ACCEPTS NUMBERS AND STOPS AFTER LAST DIGIT
//RETURNS -1 if error 
size_t							ft_satost(const char *str)
{
	int					i;
	int					sign;
	size_t 				tenpowdig;
	size_t				res;
	int					digits_sstr;

	res = 0;
	prep_atoi(str, &i, &sign);
	digits_sstr = count_digits(str, i);
	if (digits_sstr == -1)
		return ((size_t)-1);
	tenpowdig = ft_powa(10, digits_sstr) / 10;
	while (tenpowdig >= 1)
	{
		res += (str[i] - 48) * tenpowdig;
		tenpowdig /= 10;
		i++;
	}
	if (res == (size_t) -1) // Reserved for errors
		res--;
	return (res);
}

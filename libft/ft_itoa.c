/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/09 01:11:52 by sel-melc          #+#    #+#             */
/*   Updated: 2019/10/18 16:24:52 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	get_ndigits(int n)
{
	int digits;

	digits = 0;
	if (!n)
		return (1);
	while (n >= 1)
	{
		n /= 10;
		digits++;
	}
	return (digits);
}

static int	ft_powa(int n, unsigned int pow)
{
	if (pow == 0)
		return (1);
	else if (pow == 1)
		return (n);
	return (n * ft_powa(n, pow - 1));
}

static void	neg_imin_handle(int *n, int *negflag, int *iminf)
{
	*negflag = 0;
	if (*n == -2147483648)
	{
		*iminf = 1;
		*n = 2147483647;
		*negflag = 1;
	}
	else if (*n < 0)
	{
		*negflag = 1;
		*n *= -1;
	}
}

char		*ft_itoa(int n)
{
	char	*a;
	int		ndigits;
	int		i;
	int		negflag;
	int		iminf;

	i = 1;
	iminf = 0;
	neg_imin_handle(&n, &negflag, &iminf);
	ndigits = get_ndigits(n);
	if (!(a = malloc(ndigits + negflag + 1)))
		return (NULL);
	while (i <= ndigits)
	{
		a[ndigits + negflag - i] = ((n / (ft_powa(10, i - 1))) % 10) + 48;
		i++;
	}
	if (negflag)
		a[0] = '-';
	if (iminf)
		a[ndigits] = '8';
	a[ndigits + negflag] = '\0';
	return (a);
}

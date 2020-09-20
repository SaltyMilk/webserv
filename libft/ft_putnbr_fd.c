/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/09 03:14:48 by sel-melc          #+#    #+#             */
/*   Updated: 2019/10/18 16:35:02 by sel-melc         ###   ########.fr       */
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

static	int	ft_powa(int n, unsigned int pow)
{
	if (pow == 0)
		return (1);
	else if (pow == 1)
		return (n);
	return (n * ft_powa(n, pow - 1));
}

void		ft_putnbr_fd(int n, int fd)
{
	int i;
	int iminf;

	iminf = 0;
	if (n == -2147483648)
	{
		n = 2147483647;
		iminf = 1;
		ft_putchar_fd('-', fd);
	}
	else if (n < 0)
	{
		n *= -1;
		ft_putchar_fd('-', fd);
	}
	i = get_ndigits(n);
	while (i > 0)
	{
		if (iminf && i == 1)
			ft_putchar_fd('8', fd);
		else
			ft_putchar_fd((((n / (ft_powa(10, i - 1))) % 10) + 48), fd);
		i--;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 21:55:45 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/09/03 01:33:03 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	n_len(int a)
{
	int	i;

	i = 0;
	if (a < 0)
	{
		i++;
		a = -a;
	}
	while (a > 0)
	{
		a /= 10;
		i++;
	}
	return (i);
}

char	*init_str(int n)
{
	char	*str;
	int		nlen;

	nlen = n_len(n);
	str = (char *)malloc(sizeof(char) * (nlen + 1));
	if (!str)
		return (NULL);
	if (n < 0)
	{
		str[0] = '-';
		str[nlen] = '\0';
		return (str);
	}
	else
	{
		str[nlen] = 0;
		return (str);
	}
}

char	*ft_itoa(int n)
{
	char	*str;
	int		nlen;

	if (n == 0)
		return (ft_strdup("0"));
	if (n == -2147483648)
		return (ft_strdup("-2147483648"));
	nlen = n_len(n);
	str = init_str(n);
	if (!str)
		return (NULL);
	if (n < 0)
		n = -n;
	while (n != 0)
	{
		str[nlen - 1] = (n % 10) + '0';
		n /= 10;
		nlen--;
	}
	return (str);
}

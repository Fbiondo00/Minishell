/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_utils1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 15:46:49 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/23 18:22:17 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_strjoin2(char const *s1, char const *s2)
{
	size_t	i;
	size_t	j;
	char	*s3;
	int		lens2;

	i = 0;
	j = 0;
	if (!s1)
		return (0);
	if (!s2)
		lens2 = 0;
	else
		lens2 = ft_strlen((char *)s2);
	s3 = malloc(ft_strlen((char *)s1) + lens2 + 1);
	if (!s3)
		return (0);
	while (s1[i])
		s3[j++] = s1[i++];
	i = 0;
	while (s2 && s2[i])
		s3[j++] = s2[i++];
	s3[j] = 0;
	return (s3);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	i;
	size_t	j;
	char	*s3;

	i = 0;
	j = 0;
	if (!s1)
		return (0);
	s3 = malloc(ft_strlen((char *)s1) + ft_strlen((char *)s2) + 1);
	if (!s3)
		return (0);
	while (s1[i])
		s3[j++] = s1[i++];
	i = 0;
	while (s2[i])
		s3[j++] = s2[i++];
	s3[j] = 0;
	return (s3);
}

void	ft_bzero(void *s, size_t n)
{
	size_t	i;
	char	*dest;

	i = 0;
	dest = (char *)s;
	while (i < n)
	{
		dest[i] = 0;
		i++;
	}
	return ;
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t	i;
	char	*s;
	char	*s2;

	if (!dest && !src)
		return (0);
	s = (char *)dest;
	s2 = (char *)src;
	i = 0;
	while (i < n)
	{
		s[i] = s2[i];
		i++;
	}
	return (dest);
}

void	*ft_realloc(void *ptr, size_t size)
{
	return (realloc(ptr, size));
}

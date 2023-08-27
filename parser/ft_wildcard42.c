/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_wildcard42.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 03:11:16 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/27 04:12:28 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

DIR	*dir42(void)
{
	const char	*dirname;
	DIR			*dir;

	dirname = getcwd(0, 0);
	dir = opendir(dirname);
	if (dir == NULL)
	{
		perror("opendir() error");
		return (NULL);
	}
	return (dir);
}

char	*new_str_1(char *str, char *new_str, int flag)
{
	char			*tem;
	struct dirent	*entry;
	DIR				*dir;

	dir = NULL;
	dir = dir42();
	entry = readdir(dir);
	while ((entry))
	{
		flag = flag1(entry->d_name, str, -1);
		if (flag == 0)
		{
			tem = ft_strjoin2(entry->d_name, " ");
			if (!new_str)
				new_str = ft_strjoin2(tem, " ");
			else
				new_str = ft_strjoin2(new_str, tem);
			free(tem);
			entry = readdir(dir);
		}
		else
			entry = readdir(dir);
	}
	closedir(dir);
	return (new_str);
}

int	flag1(char *entry, char *str, int i)
{
	while (str[++i])
	{
		if (str[i] != '*')
		{
			if (ft_middle(str, i))
			{
				if (!ft_strchr(entry, str[i]))
					return (1);
			}
			else
			{
				if (check_left(str, i) == 1)
				{
					if (entry[i] != str[i])
						return (1);
				}
				else if (check_left(str, i) == 0)
					if (entry[ft_strlen(entry) - (ft_strlen(str) - i)]
						!= str[i])
						return (1);
			}
		}
	}
	return (0);
}

int	check_left(char *str, int i)
{
	int	y;

	y = 0;
	while (++i < ft_strlen(str))
	{
		if (str[i] == '*')
			return (1);
	}
	return (0);
	printf("return (Y:%d\n", y);
}

int	ft_stronly(const char *str, char c)
{
	while (*str)
	{
		printf("\nstr:control asterisk:%c\n", *str);
		if (*str != c)
			return (0);
		str++;
	}
	return (1);
}

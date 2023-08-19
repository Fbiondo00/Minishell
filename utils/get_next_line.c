/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 12:12:48 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/18 19:05:40 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// utilizza read per leggere il file e salva in backup (grandezza BUFFER_SIZE)
//legge buffer volte fino a che non troviamo non raggiunge \n o EOF
//salvando il risultato in stack
char	*ft_read(int fd, char *stack)
{
	char			*buffer;
	int				len;

	buffer = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buffer)
		return (NULL);
	len = 1;
	while (!ft_strchr_gnl(stack, '\n') && len != 0)
	{
		len = read(fd, buffer, BUFFER_SIZE);
		if (len < 0)
			return (ft_free(stack, buffer));
		if (!len && !stack)
			return (ft_free(stack, buffer));
		buffer[len] = '\0';
		stack = ft_strjoin_gnl(stack, buffer);
	}
	ft_free(buffer, NULL);
	return (stack);
}

// extract_line estrae la linea dallo stack ( \n incluso, se presente ) 
// e aggiunge il null byte
// lo stack può presentarsi in due casi:
// 1. contiene il char \n e forse   char successivi
// 2. può non contenere alcun \n ( caso in cui il file non ne abbia alcuno)
char	*extract_line(char *stack)
{
	size_t	i;
	size_t	is_newline;
	char	*line;

	if (stack && !stack[0])
		return (ft_free(stack, NULL));
	i = 0;
	while (stack[i] != '\n' && stack[i])
		i++;
	is_newline = ft_is_newline(stack[i]);
	line = (char *)malloc(sizeof(char) * (i + is_newline + 1));
	if (!line)
		return (NULL);
	i = 0;
	while (stack[i] != '\n' && stack[i])
	{
		line[i] = stack[i];
		i++;
	}
	if (is_newline)
		line[i++] = '\n';
	line[i] = '\0';
	if (!line[0])
		return (ft_free(stack, line));
	return (line);
}

//rimuove la la prima linea(tutti gli elementi fino a \n)
char	*ft_clean(char *stack)
{
	size_t		i;
	size_t		k;
	size_t		j;
	char		*clean_stack;

	i = 0;
	while (stack[i] != '\n' && stack[i])
		i++;
	j = 1;
	if (stack[i] == '\n')
		j = 0;
	clean_stack = (char *)malloc((ft_strlen_gnl(stack) - i + j));
	if (!clean_stack)
		return (NULL);
	k = 0;
	while (ft_strlen_gnl(stack) - 1 > i)
		clean_stack[k++] = stack[++i];
	clean_stack[k] = '\0';
	if (!clean_stack[0])
		return (ft_free(clean_stack, stack));
	ft_free(stack, NULL);
	return (clean_stack);
}

char	*get_next_line(int fd)
{
	char		*line;
	static char	*stack;

	stack = ft_read(fd, stack);
	if (!stack)
		return (NULL);
	line = extract_line(stack);
	if (!line)
		return (NULL);
	stack = ft_clean(stack);
	return (line);
}

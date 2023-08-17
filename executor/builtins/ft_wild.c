/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_wild.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 22:44:39 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/17 22:56:08 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	ft_wild(void)
{
	const char *dirname = getcwd(0, 0);
	// Open the directory
	DIR *dir = opendir(dirname);
	if (dir == NULL)
	{
		perror("opendir() error");
		return (1);
	}
	// Read the directory entries
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		printf("%s\n", entry->d_name);
	}
	// Close the directory
	closedir(dir);
	return (0);
}
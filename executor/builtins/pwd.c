/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:17:51 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/27 03:07:13 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	ft_pwd(void)
{
	char	buff[4096];

	if (getcwd(buff, sizeof(buff)) != 0)
		printf("%s\n", buff);
	else
		perror("errore pwd");
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:17:51 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/30 16:42:28 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	ft_pwd(t_node *node)
{
	char	buff[4096];

	if (getcwd(buff, sizeof(buff)) != 0)
		printf("%s\n", buff);
	else
		perror("errore pwd");
	node->shell->exit_status = 0;
}

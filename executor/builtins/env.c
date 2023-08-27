/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:03:00 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 17:32:30 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	ft_env(t_shell *shell)
{
	int	i;

	i = -1;
	while (shell->env[++i])
		printf("%s\n", shell->env[i]);
}

void	mall_env(t_shell *shell, char **env)
{
	int	i;

	i = 0;
	while (env[i++])
		;
	shell->env = malloc((i + 1) * sizeof(char *));
	i = -1;
	while (env[++i])
		shell->env[i] = ft_strdup(env[i]);
	shell->env[++i] = 0;
}

void	free_envp(t_shell *shell)
{
	int	i;

	i = -1;
	while (shell->env[++i])
		free(shell->env[i]);
	free(shell->env);
}

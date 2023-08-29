/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:18:34 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/29 02:19:15 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	updatepath(t_shell *shell)
{
	char	*new_valu;
	char	*new_str;
	char	*path_name;
	int		i;

	path_name = "PWD";
	new_valu = getcwd(0, 0);
	i = -1;
	while (shell->env[++i] != NULL)
	{
		if (ft_strncmp(shell->env[i], path_name, ft_strlen(path_name), 0) == 0)
		{
			new_str = ft_strjoin("PWD=", new_valu);
			free(shell->env[i]);
			shell->env[i] = new_str;
			free(new_valu);
			return ;
		}
	}
	shell->env = ft_realloc(shell->env, sizeof(char *) * (i + 2));
	new_str = ft_strjoin("PWD=", new_valu);
	shell->env[i] = new_str;
	shell->env[i + 1] = 0;
	free(new_valu);
	return ;
}

void	updateoldpath(t_shell *shell)
{
	char	*new_valu;
	char	*new_str;
	char	*path_name;
	int		i;

	path_name = "OLDPWD";
	new_valu = getcwd(0, 0);
	i = -1;
	while (shell->env[++i] != NULL)
	{
		if (ft_strncmp(shell->env[i], path_name, ft_strlen(path_name), 0) == 0)
		{
			new_str = ft_strjoin("OLDPWD=", new_valu);
			free(shell->env[i]);
			shell->env[i] = new_str;
			free(new_valu);
			return ;
		}
	}
	shell->env = ft_realloc(shell->env, sizeof(char *) * (i + 2));
	new_str = ft_strjoin("OLDPWD=", new_valu);
	shell->env[i] = new_str;
	shell->env[i + 1] = 0;
	free(new_valu);
}

void	updatecd(t_node *node, t_shell *shell)
{
	char	*pat;
	char	*oldpat;

	pat = ft_strdup(node->content.cmd[1]);
	oldpat = getcwd(0, 0);
	updateoldpath(shell);
	chdir(pat);
	updatepath(shell);
	free(pat);
	pat = getcwd(0, 0);
	if (ft_strcmp(pat, oldpat) || !ft_strcmp(node->content.cmd[1], "."))
	{
		free(oldpat);
		free(pat);
		return ;
	}
	else
	{
		free(oldpat);
		free(pat);
		shell->error = 1;
		shell->exit_status = 1;
		write(2, "No such file or directory\n", 26);
	}
}

int	ft_home(t_shell *shell)
{
	int		i;
	int		flag;

	i = -1;
	flag = 0;
	if (!shell->env)
	{
		shell->error = 1;
		shell->exit_builtin = 1;
		shell->exit_status = 1;
		write(2, "not access env\n", 16);
	}
	while (shell->env[++i])
	{
		if (!ft_strncmp("HOME=", shell->env[i], 5, 0))
		{
			flag++;
			break;
		}
	}
	if (!flag)
		return (0);
	chdir(ft_strchr(shell->env[i], '/'));
	return (1);
}

void	ft_cd(t_node *node, t_shell *shell)
{
	int	i;

	i = 0;
	while (node->content.cmd[++i])
		;
	if (i == 1)
	{
		updateoldpath(shell);
		if(ft_home(shell) == 0)
		{
			node->shell->exit_status = 1;
			write(2, "HOME not set\n", 13);
			return ;
		}
		updatepath(shell);
	}
	else
	{
		updatecd(node, shell);
		node->shell->exit_status = 0;
		return ;
	}
}

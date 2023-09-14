/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 21:42:40 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/14 01:23:30 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void free_matrix(char **matrix)
{
    int i;

    i = -1;
    while (matrix[++i])
        free(matrix[i]);
    free(matrix);
}

char *get_path(char *cmd, char **env)
{
    int i;
    char *temp;
    char *path;
    char **base;

    i = 0;
    while (ft_strncmp(env[i], "PATH=", 5, 0))
        i++;
    base = ft_split((env[i] + 5), ':');
    i = -1;
    while (base && base[++i])
    {
        temp = ft_strjoin(base[i], "/");
        path = ft_strjoin(temp, cmd);
        free(temp);
        if (access(path, X_OK) == 0)
        {
            free_matrix(base);
            return (path);
        }
        free(path);
    }
    free_matrix(base);
    return (NULL);
}

void ft_execve(t_node *node)
{
    char *path;

    if (node->content.cmd && node->content.cmd[0][0] != '/')
        path = get_path(node->content.cmd[0], node->shell->env);
    else
        path = node->content.cmd[0];
    if (!path)
        ft_clean_exit(node->shell, "Error: command not found\n", 127,1);
    if (execve(path, node->content.cmd, node->shell->env) == -1)
    {
        ft_clean_exit(node->shell, "Error: command not found\n", 127, 1);
    }
}

// 3 func
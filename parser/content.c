/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   content.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 20:00:20 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/09 17:58:01 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


// elabora la stringa e produce l array di stringhe
// da dare in pasto all EXECVE
// tuttavia deve ignorare le redirection che potrebbero essere d intralcio
// in quanto possono capitare in mezzo al comando.
// probabile questo problema debba esser risolto in set_redirection
// che rielabora nuovamente la raw_cmd andando a fare il trim dei char che
// ha usato per la redirection
void set_cmd(t_shell *shell, t_node *node)
{
    printf("set_cmd(non fa ancora nulla)..\n");
    (void)shell;
    (void)node;
}

// setta il content
//  l input operator può essere NULL
//  nella struttura node c è stringa partial_raw_cmd
//  che di volta in volta viene modificato.
//  in primis da get_raw_command, poi da set_redirection
// TBD: in set content ci si entra solo da set_node_cmd, corretto?
// set_node_operator può essere modificato per utilizzare set_content?
void set_content(t_shell *shell, t_node *node, int operator)
{
    if (operator)
        shell->tree->content.op = operator;
    else
    {
        // va anche a ripulire ulteriormente la stringa da passare a set_cmd
        printf("set_redirection..\n");
        set_redirection(shell, node);
        //set_cmd(shell, node); // il get_cmd di pipex ?
    }
}

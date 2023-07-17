/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   content.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 20:00:20 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/17 15:14:50 by flaviobiond      ###   ########.fr       */
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

// restituisce il num dei char da mallocare
// se trova " va avanti anche se ci sono gli spazi
// altrimenti si ferma se trova uno spazio o se trova un char di redirection
// echo ciao >q "q"
// echo ciao >q"q"
int get_len_value(t_node *node, int idx)
{
    int count;

    count = 0;
    if (node->raw_cmd[idx] == 34)
    {
        while (node->raw_cmd[++idx] != 34)
            count++;
    }
    else if (node->raw_cmd[idx] == 39)
    {
        while (node->raw_cmd[++idx] != 39)
            count++;
    }
    else
    {
        while (idx < ft_strlen(node->raw_cmd))
        {
            if (node->raw_cmd[idx] == ' ' || node->raw_cmd[idx] == '>' ||
                node->raw_cmd[idx] == '<')
                break;
            // else if (node->raw_cmd[idx] == 34 || node->raw_cmd[idx] == 39)
            // 	;
            count++;
            idx++;
        }
    }
    return (count);
}

// filtra raw_cmd e quote_idx dopo la redirection
// settando gli spazi in raw_cmd e gli zeri in quote_idx
// tali caratteri diventano neutri, ovvero non influenzano il risultato finale
// durante esecuzione codice
void set_raw_cmd_and_quote_idx(t_node *node, int start, int finish)
{
    int i;

    i = -1;
    printf("finish:%d\n", finish);
    printf("start:%d\n", start);
    while (++i < ft_strlen(node->raw_cmd))
    {
        if (i >= start && i <= finish)
        {
            node->raw_cmd[i] = ' ';
            node->quote_idx[i] = 48;
        }
    }
}

void set_content(t_node *node)
{
    // va anche a ripulire ulteriormente la stringa da passare a set_cmd
    printf("set_redirection..\n");
    set_redirection(node);
    // set_cmd(shell, node); // il get_cmd di pipex ?
}
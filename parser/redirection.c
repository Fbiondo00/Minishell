/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 15:42:52 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/23 15:32:49 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// start: index del primo char dopo operatore
void    set_value(t_node *node, int start)
{
    int i;
    int j;
    int diff;
    char *str;

    i = start;
    j = ft_strlen(node->raw_cmd) - 1;
    // printf("ft_strlen(j) - 1:%d\n", j);
    // printf("ft_strlen:%d\n", ft_strlen(node->raw_cmd));
    while (i++ < ft_strlen(node->raw_cmd))
    {
        if (node->raw_cmd[j] != ' ')
            break ;
        j--;
    }
    diff = j - start + 2;
    str = malloc(diff);
    // printf("j - start + 2:%d\n", diff);
    str[diff - 1] = '\0';
    i = -1;
    while (++i < diff - 1)
        str[i] = node->raw_cmd[start++]; 
    node->content.redir->value=str;
}

void set_redir_op(t_node *node, int idx, int num)
{
    printf("node->raw_cmd[idx]:%c\n", node->raw_cmd[idx]);
    if (node->raw_cmd[idx] == '>')
    {
        // if (!is_last_char(node, idx) && node->raw_cmd[idx + 1] == '>')
        //     node->content.redir->key = R_OUTPUT_APPEND;
        if (node->raw_cmd[idx + 1] != '\0' && node->raw_cmd[idx + 1] == '>')

            node->content.redir[num].key = R_OUTPUT_APPEND;
        else
            node->content.redir[num].key = R_OUTPUT_TRUNC;
    }
    if (node->raw_cmd[idx] == '<')
    {
        // if (!is_last_char(node, idx) && node->raw_cmd[idx + 1] == '<')
        //     node->content.redir->key = R_INPUT_HERE_DOC;
        if (node->raw_cmd[idx + 1] != '\0' && node->raw_cmd[idx + 1] == '<')
            node->content.redir[num].key = R_INPUT_HERE_DOC;
        else
            node->content.redir[num].key = R_INPUT;
    }
}


// verifica per ogni redirection ( > >> < <<)
// se è presente e se non sta nelle quotes
void set_token_redirection(t_node *node, int idx, int num)
{
    int i;
    int j;
    int len;
    char *chars;
    char *str;

    chars = "><";
    i = idx + 1; // siamo al char dopo op redirection
    while (node->raw_cmd[i] == ' ')
        i++;                      // i rappresenta il char dopo ' '
    len = get_len_value(node, i); // restituisce il num dei char inclusi.
    printf("VALUE COUNT:%d\n", len);
    str = malloc(len + 1);
    str[len] = '\0';
    j = 0;
    if (node->raw_cmd[i] == 34 || node->raw_cmd[i] == 39)
    {
        while (j < len)
            str[j++] = node->raw_cmd[++i];
        i++;
    }
    else // questo sotto funziona se non starta per " '
    {
        while (j < len)
        {
            if (node->raw_cmd[i] == 34 || node->raw_cmd[i] == 39)
            {
                len--;
                i++; // ci pensa la rimodulazione di raw_cmd a sistemare quote_idx
            }
            else
                str[j++] = node->raw_cmd[i++];
        }
    }
    // i esce fuori che è l ultimo char della str (VALUE).
    // idx rappresenta il primo char dell op redir
    // raw_cmd e quote_idx deve eliminare tutti i char tra idx e i
    printf("node->raw_cmd: %s\n", node->raw_cmd);
    printf("LAST i - 1(%d) CHAR:%c\n", i - 1, node->raw_cmd[i - 1]);
    printf("str(value): {%s}\n", str);
    printf("set key..\n");
    set_redir_op(node, idx, num); // va fatto in un while..
    printf("set value..\n");
    node->content.redir[num].value = str; // va fatto in un while..
    printf("PRE node->raw_cmd|%s|\n", node->raw_cmd);
    printf("PRE node->quote_idx|%s|\n", node->quote_idx);
    printf("modify raw_cmd & quote_idx..\n");
    if (ft_strchr(chars,node->raw_cmd[i - 2]) && ft_strchr(chars,node->raw_cmd[i - 3]))
    {
        printf("CAZZO!\n");
        idx--;
        i--;
    }
    set_raw_cmd_and_quote_idx(node, idx, i);
    printf("POST node->raw_cmd|%s|\n", node->raw_cmd);
    printf("POST node->quote_idx|%s|\n", node->quote_idx);
}

// ritorna il numero di operatori presenti nella raw_cmd
// soluzione: utilizza raw_cmd ed in_quotes()
int get_rd_size(t_node *node)
{
    int i;
    int count_redir;

    i = -1;
    count_redir = 0;
    while (++i < ft_strlen(node->raw_cmd))
    {
        if (node->raw_cmd[i] == '>' && !in_quotes(node, i))
        {
            if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '>')
                i++;
            count_redir++;
        }
        if (node->raw_cmd[i] == '<' && !in_quotes(node, i))
        {
            if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '<')
                i++;
            count_redir++;
        }
    }
    return (count_redir);
}

void set_redirection(t_node *node)
{
    int i;
    int num;

    i = -1;
    num = 0;
    node->content.kv_size = get_rd_size(node);
    printf("RD_NUMEBERS:%d\n", node->content.kv_size);
    if (node->content.kv_size)
    {
        node->content.redir = malloc(sizeof(*node->content.redir) * node->content.kv_size);
        while (++i < ft_strlen(node->raw_cmd))
        {
            if (node->raw_cmd[i] == '>' && !in_quotes(node, i))
            {
                printf("trovato un redir che inizia con >...\n");
                printf("index redir(i):%d\n", i);
                if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '>') // se <, errore?
                    i++;
                set_token_redirection(node, i, num++); //  passo i attuale
            }
            else if (node->raw_cmd[i] == '<' && !in_quotes(node, i))
            {
                printf("trovato un redir che inizia con <...\n");
                if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '<')
                    i++;
                set_token_redirection(node, i, num++); // passo i attuale
            }
        }
        printf("raw_cmd after set_redirection|%s|\n", node->raw_cmd);
        printf("quote_idx after set_redirection|%s|\n", node->quote_idx);
        if (node->content.redir->value)
        {
            i = -1;
            printf("node->content.kv_size:%d\n", node->content.kv_size);
            while (++i < node->content.kv_size)
            {
                printf("REDIR N°%d\n", i + 1 );
                printf("KEY: %d\n", node->content.redir[i].key);
                printf("VALUE: {%s}len:%d\n", node->content.redir[i].value, ft_strlen(node->content.redir[i].value));
            }
        }
    }
    else
        printf("NO REDIRECTION TO DO!\n");
}
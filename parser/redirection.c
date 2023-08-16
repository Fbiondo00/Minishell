/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 15:42:52 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/16 05:24:11 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void set_redir_op(t_node *node, int idx, int num)
{
    if (node->raw_cmd[idx] == '>')
    {
        if (node->raw_cmd[idx - 1] == '>')

            node->content.redir[num].key = R_OUTPUT_APPEND;
        else
            node->content.redir[num].key = R_OUTPUT_TRUNC;
    }
    if (node->raw_cmd[idx] == '<')
    {
        if (node->raw_cmd[idx - 1] == '<')
            node->content.redir[num].key = R_INPUT_HERE_DOC;
        else
            node->content.redir[num].key = R_INPUT;
    }
}

// ritorna la stringa andando a troncare il char node->flag
// leaks ??
char *modify_str(t_node *node, char *str)
{
    int i;
    int j;
    int k;
    char *new_str;

    i = -1;
    new_str = NULL;
    while (str[++i])
    {
        // printf("str[i:]:%c\n", str[i]);
        // printf("node->raw_cmd[%d]:%c \n", i, node->raw_cmd[i]);
        if (node->flag != -1 && str[i] == node->flag)
        {
            if (new_str)
                free(new_str);
            // printf("B\n");
            j = -1;
            k = 0;
            new_str = malloc(ft_strlen(str) - 1);
            new_str[ft_strlen(str) - 1] = '\0';
            // printf("ft_strlen(str) - 1:%d\n", ft_strlen(str) - 1);
            while (++j < ft_strlen(str) - 1)
            {
                // printf("C\n");
                if (j == i)
                {
                    // printf("j == i:%d\n", i);
                    k++;
                }
                if (k < ft_strlen(str))
                    new_str[j] = str[k++];
            }
            // str = new_str;
            // printf("END i:%d\n", i);
        }
    }
    // printf("str|%s|\n", str);
    // printf("new_str|%s|\n", new_str);
    return (new_str);
}

// verifica per ogni redirection ( > >> < <<)
// se è presente e se non sta nelle quotes
// num  è 0_based: è l index dell array delle redir
// idx è indice del char della redir (il secondo se >> o <<)

// 1 >a" u"  > POST node->raw_cmd|1     u"| ...taglia male
// 1 >a" 'u">zuzu >"oooo"
// 1 >a" 'u"ii>zuzu >"oooo"
void set_token_redirection(t_node *node, int idx, int num)
{
    int i;
    int j;
    int len;
    char *chars;
    char *str;
    char *new_str;
    
    new_str = NULL;
    chars = "><";
    i = idx + 1; // siamo al char dopo op redirection
    while (node->raw_cmd[i] == ' ')
        i++;                      // i rappresenta il char dopo ' '
    len = get_len_value(node, i); // restituisce il num dei char inclusi.
    // printf(" get_len_value(node, idx:%d):%d\n",i, len);
    str = malloc(len + 1);
    str[len] = '\0';
    j = -1;
    while (i < ft_strlen(node->raw_cmd))
    {
        if (node->raw_cmd[i] == ' ' || (ft_strchr(chars, node->raw_cmd[i]) && !in_quotes(node, i)))
            break ;
        else if (node->raw_cmd[i] == 34 && !in_quotes(node, i))
        {
            node->flag = 34;
            while (++i < ft_strlen(node->raw_cmd))
           {
               if (node->raw_cmd[i] == 34)
                break ;
               str[++j] = node->raw_cmd[i];
           }
        }
        else if (node->raw_cmd[i] == 39 && !in_quotes(node, i))
        {
           node->flag = 39;
           while (++i < ft_strlen(node->raw_cmd))
           {
                if (node->raw_cmd[i] == 39)
                    break;
                str[++j] = node->raw_cmd[i];
           }
        }
           str[++j] = node->raw_cmd[i++];
    }
    // printf("PRE_str: {%s}|ft_strlen(str):%d\n", str, ft_strlen(str));
    new_str = modify_str(node, str);
    // printf("POST_NEW_str: {%s}\n", new_str);
    if (new_str)
    {
        // printf("new esiste!\n");
        node->content.redir[num].value = new_str;
    }
    else
    {
        // printf("new non esiste!\n");
        // printf("POST_str: {%s}\n", str);
        node->content.redir[num].value = str;
    }
    set_redir_op(node, idx, num);
    
    // printf("PRE node->raw_cmd|%s|\n", node->raw_cmd);
    // printf("PRE node->quote_idx|%s|\n", node->quote_idx);
    // printf("modify raw_cmd & quote_idx..\n");
 
    // i esce fuori che è il char spazio o è finita la raw o un char redir!!
    // raw_cmd e quote_idx deve eliminare tutti i char tra idx e i
    set_raw_cmd_and_quote_idx(node, idx, i);
    // printf("POST node->raw_cmd|%s|\n", node->raw_cmd);
    // printf("POST node->quote_idx|%s|\n", node->quote_idx);
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

// controlla se è presente fd valido prima della redir
int is_fd_valid(t_node *node, int idx)
{
    int i;

    i = idx;
    while (--idx > 0)
    {
        if (node->raw_cmd[idx] == ' ')
            break ;
        if (!ft_isdigit2(node->raw_cmd[idx]))
            return (0);
    }
    return (1);
}

// idx: indice del primo operatore (anche in caso di << >>)
// num: index della redir
// modifica valore di node->content.fd:
// setta -1: in caso di assenza o in caso di valore non valido(non numero)
// setta x: in caso di numero valido
// nel secondo caso sostituisce il valore con il char space
void set_fd(t_node *node, int idx, int num)
{
    if (node->raw_cmd[idx - 1] != ' ' && is_fd_valid(node, idx))
    {
        if (!ft_isdigit2(node->raw_cmd[idx - 2]))
            node->content.redir[num].fd = node->raw_cmd[idx - 1] - 48;
        else
            node->content.redir[num].fd = -1;
        while (--idx >= 0)
        {
            if (node->raw_cmd[idx] == ' ')
                break;
            node->raw_cmd[idx] = ' ';
        }
    }
    else
        node->content.redir[num].fd = -1;
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
                set_fd(node, i, num);
                if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '>')
                    i++;
                set_token_redirection(node, i, num++); //  passo i attuale
            }
            else if (node->raw_cmd[i] == '<' && !in_quotes(node, i))
            {
                printf("trovato un redir che inizia con <...\n");
                set_fd(node, i, num);
                if (!is_last_char(node, i) && node->raw_cmd[i + 1] == '<')
                    i++;
                set_token_redirection(node, i, num++); // passo i attuale
            }
        }
    }
    else
        printf("NO REDIRECTION TO DO!\n");
}

// 5 func

// 1. fix + logic redir
//    CASI:
//    A: AGGIUNGERE SINTAX ERROR
//    casi:(&>  <>  >& | ><   &< <&)dare errore!fare set chars per redir e dare errore if in
//    A2: aggiungere anche altre syntax error di sotto(anche op)

// --syntax error--
// echo 7>1>2 è errore.
//  soluzione: in fase di check syntax nella str spacchettare a partire da ultima redir
// quindi l ultima redir si prende 1>2 e li setta a spazi.
// poi il primo redir è 7> .. mancando la parte destra da syntax error!

// --aggiungere FD--
// avendo gestito il caso precedente in syntax error, possiamo correttamente lavorare
// le redir in ordine crescente, sapendo che non ci possono essere fd che invalidano la raw

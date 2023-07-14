/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 15:42:52 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/13 00:52:39 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// TODO: in realta non e'fino a fine riga ma fino a che non trova uno spazio
void    set_key(t_node *node, int start)
{
    int i;
    int diff;
    char *str;

    i = start;
    diff = ft_strlen(node->raw_cmd) - start;
    printf("(pre)diff:%d\n", diff);
    while (++i < ft_strlen(node->raw_cmd))
    {
        if (node->raw_cmd[i] == ' ')
            diff--;
    }
    printf("(after)diff:%d\n", diff);
    str = malloc(diff + 1);
    str[diff] = '\0';
    i = -1;
    while (++i < diff)
        str[i] = node->raw_cmd[start++]; 
    node->content.redir->value=str;
}

// TODO: rework di is_last_char, che deve accettare char * come input
void set_redir_op(t_node *node, int idx)
{
    printf("node->raw_cmd[idx]:%c\n", node->raw_cmd[idx]);
    if (node->raw_cmd[idx] == '>')
    {
        printf("1\n");
        // if (!is_last_char(node, idx) && node->raw_cmd[idx + 1] == '>')
        //     node->content.redir->key = R_OUTPUT_APPEND;
        if (node->raw_cmd[idx + 1] != '\0' && node->raw_cmd[idx + 1] == '>')
        {
            printf("2\n");
            node->content.redir->key = R_OUTPUT_APPEND;
        }
        else
        {
            printf("3\n");
            node->content.redir->key = R_OUTPUT_TRUNC;
        }

    }
    if (node->raw_cmd[idx] == '<')
    {
        // if (!is_last_char(node, idx) && node->raw_cmd[idx + 1] == '<')
        //     node->content.redir->key = R_INPUT_HERE_DOC;
        if (node->raw_cmd[idx + 1] != '\0' && node->raw_cmd[idx + 1] == '<')
            node->content.redir->key = R_INPUT_HERE_DOC;
        else
            node->content.redir->key = R_INPUT;
    }
}

// INPUT:
// 1. index attuale token redir. Viene passato ultimo se >>  <<
// 2. Nodo in cui verrà aggiornata t_kv e raw_cmd
// COSA FA:
// Setta un key/value (key: tipologia redirect, value: nome file)
// Aggiorna il raw_cmd andando a togliere i char coinvolti in redirect
// COME TROVA I CHAR DEL FILE?
// se dopo il simbolo trova spazi li ignora
// comincia a leggere quando trova il primotermina quando incontra uno spazio
// N.b: IN realtà potrebbe terminare anche se trova un altro operatore
// ma la stringa che viene passata viene troncata prima della presenza
// di un operatore valido. Gli viene passata fino al char prima.
// TBD: echo >     "ciao u"a
// dal momento che potrebbero esserci piu redir, anche separate
// tra loro, una possibile soluzione:
// fare malloc parziali ed ogni volta fare il free della raw_cmd.
// ogni volta la nuova stringa contiene gli stessi char tranne quelli
// coinvolti nella redirection
// inoltre va creata la kv:
// un altra stringa contenente la sola parte i - idx rappresenta il file(value)
// get_redir_op setta la key
void set_token_redirection(t_shell *shell, t_node *node, int idx)
{
    int i;
    int j;
    int k;
    int len;
    char *str;

    i = idx + 1;
    len = ft_strlen(node->raw_cmd);
    printf("i(pre space):%d\n", i);
    printf("len of (node->raw_cmd):%d\n", len);
    printf("is_last_char(shell, i):%d\n", is_last_char(shell, i));// errore, deve check raw_cmd e non rawline!
    while (!is_last_char(shell, i - 1) && node->raw_cmd[i] == ' ')
        i++;
    printf("i(after space):%d\n", i);
    if (len == i)
        return;
    str = malloc(len - (i - idx));
    str[len - (i - idx)] = '\0';
    printf("len - (i - idx):%d\n", len - (i - idx));
    j = 0;
    k = 0;
    while (k++ < len)
    {
        if (j < idx || j > i - 1)
        {
            printf("|%c|",node->raw_cmd[j]);
            str[j] = node->raw_cmd[j];
            j++;
        }
    }
    printf("\n");
    printf("str: %s\n", str);
    printf("node->raw_cmd: %s\n", node->raw_cmd);
    printf("set_key..\n");
    printf("idx: %d | i(start key):%d\n", idx, i);
    set_key(node, i);
    printf("set_redir_op..\n");
    set_redir_op(node, idx);
    free(node->raw_cmd);
    node->raw_cmd = str;
}
// ritorna il numero di operatori presenti nella raw_cmd
// soluzione: utilizza raw_cmd ed in_quotes()
int get_rd_size(t_node *node)
{
    (void)node;
    return (3);
}

// verifica per ogni redirection ( > >> < <<)
// se è presente e se non sta nelle quotes
// CASI DA GESTIRE:
// 1.  echo a < (echo a >)(echo a >>)
//  bash: syntax error near unexpected token `newline'
// file1 non esiste
// 2. echo a <file1
//  bash: u: No such file or directory
// 3. echo a >file1 (echo a >>file1)   -->crea il file con successo
//  echo a > "Desktop/e u"
//  bash: Desktop/e u: No such file or directory
//  echo a > "/Desktop/e u"
//  bash: /Desktop/e u: No such file or directory
//  MORALE: ->se vede / pensa che è un path assoluto, anche con apici singoli...
// set_redirection va a modificare raw_cmd, togliendo la stringa
// relativa alla redirection, poi va a settare il t_kv
// successivamente l executor potrà eseguire tali redirection quando
// l algoritmo ritiene opportuno.
// SOLUZIONE:
// per ogni token di redirection individuato viene mallocato un
// key-value pair contentente il tipo di redirection e nome del file
void set_redirection(t_shell *shell, t_node *node)
{
    int i;

    i = -1;
    node->content.kv_size = get_rd_size(node);
    node->content.redir = malloc(sizeof(*node->content.redir) * node->content.kv_size);
    // node->content.redir[get_rd_size(node)] = NULL; ??
    while (++i < ft_strlen(node->raw_cmd))
    {
        if (node->raw_cmd[i] == '>' && !in_quotes(shell, i))// index coincidono raw_cmd e rawline?
        {
            printf("trovato un redir che inizia con >...\n");
            printf("index redir(i):%d\n", i);
            set_token_redirection(shell, node, i); //  passo i attuale
            // if (!is_last_char(shell, i) && node->raw_cmd[i + 1] == '>')
            //     set_token_redirection(shell, node, i + 1); // passo i attuale / INPUT APPEND
            // else
            //     set_token_redirection(shell, node, i); //  passo i attuale / OPEN TRUNC
        }
        else if (node->raw_cmd[i] == '<' && !in_quotes(shell, i)) // se non è presnte
        {
            printf("trovato un redir che inizia con <...\n");
            set_token_redirection(shell, node, i); // passo i attuale
            // if (!is_last_char(shell, i) && node->raw_cmd[i + 1] == '<') // seg fault?
            //     set_token_redirection(shell, node, i + 1); // passo i attuale /HERE_DOC
            // else
            //     set_token_redirection(shell, node, i); // passo i attuale /INFILE
        }
    }
    printf("raw_cmd after set_redirection:\n");
    printf("%s\n", node->raw_cmd);
    if (node->content.redir->value)
    {
        printf("KEY: %d\n", node->content.redir->key);
        printf("VALUE: |%s|len:%d\n", node->content.redir->value, ft_strlen(node->content.redir->value));
    }
}
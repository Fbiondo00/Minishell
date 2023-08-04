/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/04 20:18:33 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// EXECUTOR
// A. lancia il comando
// QUESTE OPERAZIONI IN FASE DI EXEC o PARSER?
// in generale le operazioni sono in questo ordine:
//  tilde expansion (NON fare)
//  parameter expansion (SI)
//  command substitution (NON fare)
//  arithmetic expansion (NON fare)
//  pathname expansion (??)
//  quote removal
//  alias substitution (in che ordine?)
// B. setta stdin/stdout/stderr se modificate dalle redirection
// -------------------------------------------------------------
// TODO:
// Per debug
//     -creare funzione che stampa il nodo ed i suoi attributi
//     -creare funzione che naviga i nodi
//     -creare funzione che accede al nodo di start execution, ovvero
//         all ultimo nodo riscontrato fra tutte le left_branchç



t_node  *get_starter_node(t_shell *shell)
{
    t_node *nod;

    nod = NULL;
    if (shell->tree)
    {
        nod = shell->tree;
        while (nod)
        {
            if (!nod->left)
               break ;
            nod = nod->left;
        }
        printf("p:%p\n", nod);
    }
    return (nod);
}

/*
// in generale vengono sempre eseguite tutte le  redirections prima del lancio dei
// comandi.
// echo a>b|echo b|echo c && pi zi ||   pippo <<u z
// Risultato:
// > d (here_doc)
// > u
// c
// bash: pi: command not found
// bash: pippo: command not found

// https://www.cs.colostate.edu/~mcrob/toolbox/unix/redirection#:~:text=File%20redirection%20happens%20second%2C%20and,The%20file%20redirection%20always%20wins.)

// tra pipe redir e file redir vince sempre il file redir.
// questa funzione esegue il file redir di tutti i nodi_cmd tranne quelli
// coinvolti con pipe. Ovvero quelli il cui back o right node sia pipe
void execute_initial_redirections(t_node *node)
{
    while (node->back || node->right)
    {

    }
}

void    execute(t_shell *shell)
{
    t_node *node;

    printf("----EXECUTE PHASE----\n\n");

    printf("Shell->tree address:%p\n", shell->tree);
    printf("Calculating starting node...\n");
    node = get_starter_node(shell);
    printf("Start node address:%p\n", node);
    printf("Start node cmd[0]:%s\n", node->content.cmd[0]);
    printf("Execute initial file redirections...\n\n");
    // esegue le redirections di tutti i nodi_cmd
    execute_initial_redirections(node);
    // if (is_builtins(node))
    while (node->back || node->right)
    {

    }
}
*/


// ------------------INSERT CODE HERE!--------------------------
// attenzione al subject:
// a volte specifica senza options altre volte senza arguments.
// come gestire i vari casi?
// definire i metodi nei rispettivi file..
void execute_builtin(t_node *node, t_shell *shell)
{
    int len;

    len = ft_strlen(node->content.cmd[0]);
    if (!ft_strncmp(node->content.cmd[0], "echo", len))
        ft_echo(node);
    if (!ft_strncmp(node->content.cmd[0], "pwd", len))
        ft_pwd();
    else if (!ft_strncmp(node->content.cmd[0], "env", len))
         ft_env(shell);
    // else if (!ft_strncmp(node->content.cmd[0], "export", len))
    //     // ft_export(node);
    // else if (!ft_strncmp(node->content.cmd[0], "unset", len))
    //     // ft_unset(node);
    // else if (!ft_strncmp(node->content.cmd[0], "exit", len))
    //     // ft_exit(node);
    else if (!ft_strncmp(node->content.cmd[0], "cd", len))
        ft_cd(node);
}

// ritorna 1 se è un comando builtin
// ritorna 0 in caso contrario
int is_builtin(t_node *node)
{
    int len;

    len = ft_strlen(node->content.cmd[0]);
    if (!ft_strncmp(node->content.cmd[0], "echo", len))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "pwd", len))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "env", len))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "export", len))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "unset", len))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "exit", len))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "cd", len))
        return (1);
    return(0);
}

// funzione demo per lanciare i comandi builtins
// attualmente da lanciare senza caratteri () , >> , <<
// questa versione dell execute:
// 1. puo eseguire solo un comando per volta, quindi non vanno messi gli operatori
// 2. likkerà ezy, ma non è importante al momento
void execute_demo(t_shell *shell)
{
    t_node *node;

    printf("----EXECUTE PHASE----\n");
    printf("----Daje flavié!!----\n\n");

    printf("Shell->tree address:%p\n", shell->tree);
    printf("Calculating starting node...\n");
    node = get_starter_node(shell);
    // printf("Start node indirizzo:%p\n", node);
    // printf("Start node cmd[0]:%s\n", node->content.cmd[0]);
    // printf("Start node cmd[1]:%s\n", node->content.cmd[1]);
    // printf("Start node cmd[2]:%s\n", node->content.cmd[2]);
    if (is_builtin(node))
    {
        printf("eseguo comando builtin...\n");
        execute_builtin(node, shell);
    }
    else
        printf("Non è un comando builtin!\n");
    exit(99);
}

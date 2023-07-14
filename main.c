/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 22:20:06 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/13 01:30:27 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


//1.  LEXER 
    //A. manda il prompt quando aspetta per un nuovo comando
        // viene interrotto dai segnali CTRL+D CTRL+C  (CTRL+\ non deve mandare alcun segnale?)
            // intercettare segnale con signal (no thread)
    //B. una volta ricevuto la linea di comando (da cosa viene terminata?)
        // controlla se errore delle " o ' non chiuse
    //C. TOKEN RECOGNITION ?
//2. PARSER
    //A. IDENTIFY literal value from special value of chars.
        // Distinguish between
        // WORDS
        // OPERATOR
        // PRECEDENCE ORDER 
        // order of : echo ciao && echo come | echo stai 
        // first | than &&, so:
        //echo ciao && (echo come | echo stai)
        // echo ciao && {echo come | echo stai} (uguale )

        // ORDINE REDIRECTION
        // echo>test2 ciao == echo ciao >test2 == >test2 echo ciao
        
        //--> PER POTER IDENTIFICARE ORDINE BISOGNA LEGGERE TUTTA LE RIGA 
        // E MAPPARE TUTTI GLI OPERATORI E PAROLE.
    
    //B. fa le operazioni necessarie in questo ordine
        // tilde expansion
        // parameter expansion
        // command substitution
        // arithmetic expansion
        // pathname expansion
        // quote removal
        // alias substitution (in which order?)
    //C. setta stdin/stdout/stderr se modificate dalle redirection
//3. EXECUTOR
    //A. lancia il comando

#include "minishell.h"

char *ft_strchr(char *s, char ch)
{
    int slen;
    int i;

    i = 0;
    slen = ft_strlen(s);
    while (i <= slen)
    {
        if (s[i] == ch)
            return ((char *)&s[i]);
        i++;
    }
    return (NULL);
}

int ft_strlen( char *s)
{
    int i;

    i = 0;
    while (s[i])
        i++;
    return (i);
}

// setta i valori iniziali
// salva l environment iniziale, history a 0
// attivazione signal
// possiamo assegnare e modificare direttamente env o bisogna fare una copia?
void    shell_init(int argc, char **argv, char **env, t_shell *shell)
{
    (void) argv;
    if (argc != 1)
        exit(write(1, "Error: invalid arguments\n", 25) - 24);
    shell->is_alive = 1;
    shell->temp_input = dup(STDIN_FILENO);
    shell->temp_output = dup(STDOUT_FILENO);
    shell->temp_error = dup(STDERR_FILENO);
    shell->env = env;// get_env(env)
    shell->rawline = NULL;
    shell->quote_idx = NULL;
    shell->tree = NULL;
}

void    ft_exit(t_shell *shell, char *str)
{
    int len;
    
    len = ft_strlen(str);
    if(shell->quote_idx)
        free(shell->quote_idx);
    if(shell->rawline)
        free(shell->rawline);
    write(1, str, len);
    exit(1);
}

//  echo a && echo b | (false && echo d | echo e)             OK > a
//  echo a && echo b | echo c ( false && echo d | echo e )    KO
//  echo a && echo b | echo c (&&  false && echo d | echo e ) KO
//  echo a && echo b | echo c &&(  false && echo d | echo e ) OK > a c
//  echo ok || echo zi && echo ciao || << ko         se cè here_doc lo fa sempreper primo.
//TODO:init tutti i default value in struct. Ex: pointer a NULL
int main(int argc, char **argv, char **env)
{
    t_shell shell;
    
    shell_init(argc, argv, env, &shell);
    while(shell.is_alive)
    {
        ft_read_line(&shell);
        if (shell.rawline && shell.rawline[0]) //anche in ft_read_line?
            set_tree(&shell);
        // print_node(&shell.tree[0]);
        // play(&shell);
    } 
}

// TEST a> b|c
// -fsanitize=address
// seg fault ogni tanto con comandi semplici ripetuti
// a| ??  check_op_logic_than_pipe non controlla ultimo ne primo char?
// inizializzare bene tutti i valori
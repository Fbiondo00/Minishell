/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 22:20:06 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/06 15:11:36 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// stampa errore e ritorna exit status desiderato
int write_error(char *str, int exit_status)
{
    write(1, str, ft_strlen(str));
    return (exit_status);
}

// to_exit == 1 se bisogna fare anche exit, altrimenti == 0 per fare solo clean
void ft_clean_exit(t_shell *shell, char *str, int exit_status, int to_exit)
{
    if (shell->quote_idx)
        free(shell->quote_idx);
    if (shell->rawline)
        free(shell->rawline);
    if (shell->str)
        free(shell->str);
    if (to_exit)
        exit(write_error(str,exit_status));
}

// setta i valori iniziali
// salva l environment iniziale
// attivazione signal
// possiamo assegnare e modificare direttamente env o bisogna fare una copia?
void shell_init(int argc, char **argv, char **env, t_shell *shell)
{
    (void) argv;
    shell->is_alive = 1;
    shell->temp_input = dup(STDIN_FILENO);
    shell->temp_output = dup(STDOUT_FILENO);
    shell->temp_error = dup(STDERR_FILENO);
    shell->env = NULL;
    shell->rawline = NULL;
    shell->quote_idx = NULL;
    shell->tree = NULL;
    shell->str = NULL;
    if (argc != 1)
        ft_clean_exit(shell, INVALID_ARGS, 1, 1);
    ft_signals(shell);
    welcomeScreen();
    mall_env(shell, env);
}

int in_quotes(t_node *node, int index)
{
    int i;
    int d_quotes;
    int s_quotes;

    i = -1;
    d_quotes = 0;
    s_quotes = 0;
    while (++i < index)
    {
        if (node->quote_idx[i] == 34)
            d_quotes++;
        if (node->quote_idx[i] == 39)
            s_quotes++;
    }
    if (s_quotes % 2 != 0)
        return (-1);
    if (d_quotes % 2 != 0)
        return (1);
    return (0);
}

// accorciare main
int main(int argc, char **argv, char **env)
{
    t_shell shell;
    
    shell_init(argc, argv, env, &shell);
    mall_env(&shell, env);
    while(shell.is_alive)
    {
        ft_read_line(&shell);
        if (shell.rawline && shell.rawline[0]) //anche in ft_read_line?
        {
            set_tree(&shell);
            // tree_of_life(&shell);
            // execute(&shell);
            execute_demo(&shell);
            ft_clean_exit(&shell, NULL , 3, 0);
        }
    }
}



//----------------------------TODO: ----------------------------------
// 1. fix parentheses
// 2. fix + logic redir
// 3. executor & subshells
// 4. rework parser: lvl_subshell + clean () signs
// 5. comandi valgono anche se passati in maiuscolo (echo/ECHO - ls/LS)
// ------------------- WIP -------------------
// casi rimanenti parentesi:
// syntax nested parentheses:
// 1. se ((ls)) o (())  ((ee)) --> fare niente, non controllare nemmeno se non esiste comando
// ovvero non syntax error ma ignora
// 1.a(((ls)))   di fatto dovrebbe essere pure dinamico... questo rientra nel caso 
// 1.b ((ls) ), secondo questa regola questo viene eseguito. e di fatto e'corretto!
// non rientra nel caso precedente.
// viene controllato all inizio, il caso non arriva alla funzione ricorsiva
// il check potrebbe essere se sono 2 ( e se i primi due idx sono (( e ultimi due sono ))
// 2. BUG: ((ls) | (ls))
// 3. BUG: ( |pwd)  --> lo stesso check per le ( ) va fatto anche se non sono presenti


// fix redir
//      A. fix simple redir a|b>d<e ( sballa valori  redir e seg fault.)
//      B. fix >> << in set_redirection (a>b<c<<d  bug redir??)
//      C. considerare 7>a  valido ma ignorare il numero prima, ovvero rendero renderlo blank (ovvero ignorare il numero)
//      D. <> dare errore
///     E. &> errore ?

// CASI DA GESTIRE REDIRECTION (set_token_redirection) :
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

// idx corrispondente all ultimo char dell operatore
// gestione ""? <>?
// andrebbe creato un metodo che torna la lunghezza in base ai char che vengono trovati
// ----
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

// (echo b || echo a >e) non fa redir

//----------------------------TBD: ----------------------------------
// 1. lasciare le () in quote_idx, per determinare le subshell nell executor
// 2. ft_read_line()
// 3. LEAKS READLINE?
// https: // stackoverflow.com/questions/55196451/gnu-readline-enormous-memory-leak
// ...and the history can be freed calling void rl_clear_history(void), add that function call in your program and redo a test
// Se è builtin va fatto uguale il fork o si esegue dal main?
// 4. controllo questa affermazione per  set_value() è ancora valida
// TODO: in realta non e'fino a fine riga ma fino a che non trova uno spazio
// in realtà questa func setta il value e in il key
// 5. fare expansion delle variabili! (in che fase vengono effettuate le expansions?)
// 6. fix print_tree
//----------------------------INFO VARIE: ---------------------------
// 1. DEBUG
// gdb -tui ./minishell > c > run
//  -fsanitize=address

// 2. COMPORTAMENTO REDIR BASH
// IN generale se primo o ultimo redir input non esiste
//  non viene considerato il secondo (si ferma prima)
// e torna errore
// bash-3.2$ ls -la <aa <u | cat
// bash: u: No such file or directory

// 3.  UTILIZZO DELLE PARENTESI TONDE
// https://unix.stackexchange.com/questions/267660/difference-between-parentheses-and-braces-in-terminal
// VALID
// EX1. echo a | (>uu)
// check se cmd  è vuota (O SE TUTTI SPAZI)non lanciare l execve ma uscire diretto
// EX2.  (<<u)|echo a
// INVALID
// ( ) | echo a
// ci deve essesere un char diverso da spazio
// bash: syntax error near unexpected token `)'

// =(echo b || echo a >e) non fa redir

// 4. https://www.shellcheck.net
// (ls) c
// (error): Unexpected tokens after compound command. Bad redirection or missing ;/&&/||/|?
//  (ls) <u


// ------------------- TESTER -------------------
// echo c|
// |echo c
// (|(echo c))
// ((echo c)|)
// (|echo c)
// (echo c|)
// ((ls))
// ((ls) )
// (())
// ((ls) | (ls))
// ((dispari)
// <a (ls)
// echo a | (>uu)
// (<<u)|echo a
// (echo b || (echo a ) >u) non fa redir
// (echo b || echo a >e) non fa redir

// ((echo a && ls))  bug
// (echo a && ls)
// (echo a && ls) | cat

// (echo a && (echo b && (echo c))) >p| cat
// (echo a && (echo b && (echo c))>p) | cat
// ----
// export v="test" && (echo $v && v="modified" && echo $v) && echo $v
//  (ls) <u
// ----
//  echo a && echo b | (false && echo d | echo e)             OK > a
//  echo a && echo b | echo c ( false && echo d | echo e )    KO
//  echo a && echo b | echo c (&&  false && echo d | echo e ) KO
//  echo a && echo b | echo c &&(  false && echo d | echo e ) OK > a c
//  echo ok || echo zi && echo ciao || << ko         se cè here_doc lo fa sempreper primo.
// ----
// get_len_value()
// echo ciao >q "q"
// echo ciao >q"q"
// ----
//  (echo a | echo b && echo c )| echo d
//  echo a | echo b && echo c | echo d
//  (echo a | echo b | echo c >zi ) >zu && echo d


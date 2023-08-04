/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 22:20:06 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/04 20:16:16 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// stampa errore e ritorna exit status desiderato
int write_error(char *str, int exit_status)
{
    write(1, str, ft_strlen(str));
    return (exit_status);
}

void ft_clean_exit(t_shell *shell, char *str, int exit_status)
{
    (void)shell;
    if (shell->quote_idx)
        free(shell->quote_idx);
    if (shell->rawline)
        free(shell->rawline);
    exit(write_error(str,exit_status));
}

// setta i valori iniziali
// salva l environment iniziale
// attivazione signal
// possiamo assegnare e modificare direttamente env o bisogna fare una copia?
void    shell_init(int argc, char **argv, char **env, t_shell *shell)
{
    (void) argv;
    // if (argc != 1)
    //     exit(write_error(INVALID_ARGS, 1));
    shell->is_alive = 1;
    shell->temp_input = dup(STDIN_FILENO);
    shell->temp_output = dup(STDOUT_FILENO);
    shell->temp_error = dup(STDERR_FILENO);
    shell->env = env;// get_env(env)
    shell->rawline = NULL;
    shell->quote_idx = NULL;
    shell->tree = NULL;
    if (argc != 1)
        ft_clean_exit(shell, INVALID_ARGS, 1);
    ft_signals(shell);
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

int main(int argc, char **argv, char **env)
{
    t_shell shell;
    
    shell_init(argc, argv, env, &shell);
    welcomeScreen();
    while(shell.is_alive)
    {
        ft_read_line(&shell);
        if (shell.rawline && shell.rawline[0]) //anche in ft_read_line?
            set_tree(&shell);
        // tree_of_life(&shell);
        //execute(&shell);
        execute_demo(&shell);
    }
}

//----------------------------TODO: ----------------------------------
// 1. creare directory utils  [DONE]
// 2. check syntax
    // A. primo/ultimo char è un operatore (spazi si skippano) [DONE]
    //    check iniziale/ dare errore se idx 0 o idx len -1 è un operatore
    //    Ex.  echo c|   o  |echo c..
    // B. deve esserci un numero pari di () valide
    // C. dopo () deve essere presente  redirection o operatore [in progress...]
    // ((ls)) ???
// 3. fix redir
//      A. fix simple redir a|b>d<e ( sballa valori  redir e seg fault.)
//      B. fix >> << in set_redirection (a>b<c<<d  bug redir??)
// 4. fix print_tree
// 5. init tutti i default value in struct. Ex: pointer a NULL
// 6. fare expansion delle variabili!
// 7. controllo questa affermazione per  set_value() è ancora valida
// TODO: in realta non e'fino a fine riga ma fino a che non trova uno spazio
// in realtà questa func setta il value e in il key
// 8. recuperare definizione di in_quotes
//----------------------------TBD: ----------------------------------
// lasciare le () in quote_idx, per determinare le subshell nell executor
// 1. se a <>b dare  errore?
// 2. ft_read_line()
// LEAKS READLINE?
// https: // stackoverflow.com/questions/55196451/gnu-readline-enormous-memory-leak
// ...and the history can be freed calling void rl_clear_history(void), add that function call in your program and redo a test
// Se è builtin va fatto uguale il fork o si esegue dal main?

//----------------------------INFO VARIE: ---------------------------
// 1. DEBUG
// gdb -tui ./minishell > c > run
//  -fsanitize=address

// 2. COMPORTAMENTO REDIR BASH
// IN generale se primo o ultimo redir input non esiste
//  non viene considerato il secondo
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

// 4. https://www.shellcheck.net
// (ls) c
// (error): Unexpected tokens after compound command. Bad redirection or missing ;/&&/||/|?
//  (ls) <u

//----------------------------CHECK CASES: ---------------------------

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

//  (echo a | echo b && echo c )| echo d
//  echo a | echo b && echo c | echo d

//  (echo a | echo b | echo c >zi ) >zu && echo d

// concettualmente è come se l output dei comandi in subshell viene redirectato
// in un file provvisorio se non esiste oppure in un file indicato fuori tonde
// ma cmq si possono specificare file redir anche internamente le tonde

//  (cat | cat | cat  >zi ) <du  && echo d
// se specificato <du diventa l input del primo cmd e viene inserito in >zi
// quindi il risultato finale è solo "d"

// (cat <zu | cat | cat  >zi ) <du  && echo d
// in questo caso vince zu che viene aperto per secondo

// (cat && ls >a) <du  >u && echo d //  con o senza >u cambia tutto
// perche leva il risultato di tale redirezione

// da notare che l && interno non fa mostrare output sul terminale ma viene ridirezionato
// in file provvisorio con APPEND!!e poi dato in output al cmd dopo le parentesi

// redir > si comporta da >> in caso venga piazzata al di fuori delle tonde e ha
// valenza per tutti i nodi_cmd

//((ls))
//--------------------------------------------------------------------------------

// SOLUZIONE:
// aggiungere flag is_parentheses al nodo_operatore che fa da spartiacque tra le varie 
// liste cmd da fare in subshell.

//EX. (a | b | u && h |c | d) | (e && f)
//flag:  1   1   1    1  1     0    1 

// una volta configurata, quando andremo a cercare i comandi da inserire il lista
// partendo dallo starter node andremo a verificare la flag.
// in caso stia ad 1 verifico se il nodo_operatore precedente stia sempre ad 1 e cosi via.
// quando troviamo il primo 0, raggruppiamo tutti i nodi fino a quel momento sotto un unica 
// subshell.
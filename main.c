/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 22:20:06 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/30 05:22:18 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// to_exit == 1 se bisogna fare anche exit
// to_exit == 0 per fare solo clean
void ft_clean_exit(t_shell *shell, char *str, int exit_status, int to_exit)
{

    if (shell->quote_idx)
    {
        printf("shell->quote_idx:%p\n", shell->quote_idx);
        free(shell->quote_idx);
        shell->quote_idx = NULL;
    }
    if (shell->rawline)
    {
        printf("shell->rawline:%p\n", shell->rawline);
        free(shell->rawline);
        shell->rawline = NULL;
    }
    if (shell->str)
    {
        printf("shell->str:%p\n", shell->rawline);
        free(shell->str);
        shell->str = NULL;
    }
    if (shell->tree)
    {
        if (is_node_cmd(shell->tree))
        {
            printf("ft_remove_heredoc(shell->tree);\n");
            ft_remove_heredoc(shell->tree);
        }
        else
        {
            printf("ft_remove_heredoc(go_to_starter_node(shell->tree->left));\n");
            ft_remove_heredoc(go_to_starter_node(shell->tree->left));
        }
    }
    // if (shell->tree)
        // free_tree(shell);  friare tutto l albero navigazione nodi, poi free.
    // mi perdo l exit status se free, salvare in var locale?
    if (str) //cazzata exit_status??
        shell->exit_status = write(2, str, ft_strlen(str)) - ft_strlen(str) + exit_status;
    printf("fine line: shell->exit_status:%d\n", shell->exit_status);
    if (to_exit) // dovrebbe essere chiamata solo da builtin exit..
    {
        free_envp(shell);
        // ft_reset_original_fd(); ??
        
        close(shell->temp_input);
        close(shell->temp_output);
        close(shell->temp_error);
        exit(exit_status);
    }
}

// ritorna  1 in caso sia presente nelle double quotes "
// ritorna -1 in caso sia presente nelle single quotes '
// ritorna  0 se non in quotes
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
    if (argc != 1)
        exit(write(2, "Error: invalid arguments\n", 25) - 24);
    shell_init(argc, argv, env, &shell);
    while(1)
    {
        ft_read_line(&shell);
        if (shell.rawline && shell.rawline[0]) //anche in ft_read_line?
        {
            set_tree(&shell);
            set_components(&shell);
            execute(&shell);
            ft_clean_exit(&shell, NULL , 3, 0);
        }
       // ci entra ogni volta che fai invio senza scrivere nulla
    }
}

// 5 func
//----------------------------TODO: ----------------------------------
//-3 norme: modify con flag 1,2,3 ... if 1, do_check1
//-2. exe. single_cmd subredirs
//-1. 
// 0.MAIN
// leaks
// 1.EXECUTOR
    // A.do subshell
        //  potrebbero esserci dei buchi nei lvl, non seg, da far aprire a subshell
        //  echo a && echo b



// bash-3.2$ unset HOME
// bash-3.2$ cd
// bash: cd: HOME not set

// dubbi su norminette:
// 1.SIGNAL:
//  a.  void	ft_reset_signal(void) perche?
//  b.  shell->error se è da settare errore, come?
// 2.UTILS:
//  a. get_idx_eq_st2  da cancellare?
// 3.BUILTINS
//  a. unset.c  , setta exit status in clean exit?
//----------------------------TBD: ----------------------------------
// 1. ft_read_line() LEAKS READLINE?
// https: // stackoverflow.com/questions/55196451/gnu-readline-enormous-memory-leak
// ...and the history can be freed calling void rl_clear_history(void), add that function call in your program and redo a test

// https://www.cs.colostate.edu/~mcrob/toolbox/unix/redirection#:~:text=File%20redirection%20happens%20second%2C%20and,The%20file%20redirection%20always%20wins.)

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

// 4. https://www.shellcheck.net

// in generale vengono sempre eseguite tutte le  redirections NECESSARIE prima del lancio dei
// comandi.
// echo a>b|echo b|echo c && pi zi ||   pippo <u z
// Risultato:
// > d (here_doc)
// > u
// c
// bash: pi: command not found
// bash: pippo: command not found

// echo a>b|echo b|echo c && pi zi &&   pippo <u z
// c
// zsh: command not found: pi
// non dice che u non esiste perche neanche la lancia. rientra nel ramo non lanciato.

// come nel caso: // (echo b || echo a >e) non fa redir

// ------------------- TESTER: OK/KO in base a risultati bash -------------------
// --------------- WIP -------------------

// --------------- FATTI ------------------
// echo c|       KO
// |echo c       KO
// (|(echo c))   KO
// ( |pwd)       KO
// ((echo c)|)   KO
// (|echo c)     KO
// (echo c|)     KO
// (())          KO
// ((dispari)    KO
// a | | echo b  KO
// (ls) c        KO
// <a (ls)       KO
// echo a (echo b) KO
// echo ||& echo a KO
// a &&& b       KO
// a |& b        KO
// a>& b         KO
// a &>b         KO
// a <> b        KO
// a |> b        OK
// ((ls))        OK bash non da risultato ma trattato come sotto
// ((ls) )       OK
// (((ls)))      OK
// ((ls) | (ls)) OK
// (ls) <u       OK
// echo a |(>uu) OK
// (<<u)|echo a  OK
// ("echo')")    OK
// echo a>b      OK
// a|b>d<e       OK
// echo >a>b>cc<<ddd                               OK
// echo a >"bb "c>y                                OK
// a >u>>og <<o"'  pp" p                           OK
// (echo b || (echo a ) >u)                        OK
// (echo b || echo a >e)                           OK
// ((echo a && ls))                                OK
// (echo a && ls)                                  OK
// (echo a && ls) |cat                             OK
// (echo b || echo a >e)                           OK
// (echo a && (echo b && (echo c <z))) >p| cat     OK
// (echo a && (echo b && (echo c <u))>p) | cat     OK
//  echo a || echo b | cat <<2                     OK
// ---
// echo a || echo b | echo c | echo d && echo e    OK
// echo a &&echo b | echo c                        OK
// echop a && echo b | echo c | echo d && echo e   OK
// echop a && echo b | echo c | echo d || echo e   OK
// io" echo a && echo ba" u
// bash: io echo a && echo ba: command not found
// io " echo a && echo ba" u
// bash: io: command not found
// --
// ((ls) >zy | echo a && echo c) | cat
// (cat | cat | cat  >zi ) <du  && echo d
// (cat <zu | cat | cat  >zi ) <du  && echo d
// echo a >b || echo b >c (ma anche >>c)  => non crea c, perche non esegue il comando
// echo a >b || echo b <<c   solo nel caso dell here_doc lo esegue comunque
// echo a >b || (echo b >c && echo d >e) => non crea c & e perche non esegue il cmd
// echo a>1|(echo b>2||echo c  >3) |out:  |1:a |2:b  // il file 3 non viene creato

// --
// echo a <      KO
// (echo a >)    KO
// (echo a >>)   KO
// echo a <<     KO
// echo ciao >q "r" OK
// echo ciao >q"r"  OK

// se fallisce calcolo redir input, non viene eseguito il cmd
// EX. echo a <1

// ----
// export v="test" && (echo $v && v="modified" && echo $v) && echo $v  OK

// echo a > "/Desktop/e u"
//  bash: /Desktop/e u: No such file or directory

//  echo a > "Desktop/e u"  
//  bash: Desktop/e u: No such file or directory
// ----
//  echo a && echo b | (false && echo d | echo e)             OK > a
//  echo a && echo b | echo c &&(  false && echo d | echo e ) OK > a c
//  echo ok || echo zi && echo ciao || << gg                  OK 
//  echo a && echo b | echo c ( false && echo d | echo e )          KO
//  echo a && echo b | echo c (&&  false && echo d | echo e )       KO
// echo a && echo b | echo c |( false && echo d | echo e ) echo c   KO
// ----
//  (echo a | echo b && echo c )| echo d                            OK
//  echo a | echo b && echo c | echo d                              OK
//  (echo a | echo b | echo c >zi ) >zu && echo d                   OK

// -- test components --
// (ECHO"b" && (ECHO "a" && (ECHO "d")))|ECHO "c"                   OK
// ((echo a|echo b|echo u&& echo h|echo c|echo d)|(ECHO E&&ECHO F))&&(G|LS O|LS U) OK

// echo a >1 && ( echo b >2 || echo c>3) || echo d >4  (crea 1 e 2) OK

// echo a >1 && ( echo b >2 || echo c <<3) || echo d <<4            OK
// esegue tutti here_doc a prescindere.

// bash-3.2$ echo a| (cat ||echo c  >3 ) | echo d<4                 OK
// bash : 4 : No such file or directory

// bash-3.2$ echo a| (cat ||echo c  >3 ) && echo d<4                OK
// a
// bash: 4: No such file or directory

// ((((echo a)>1)>2)>3)  il valore finisce in 1                     OK

// priorità input in subshell
// caso PIPE(cmd partenza lvl.0):
// next_cmd : è in fork, input viene dato solo al primo cmd della subshell

// bash-3.2$ echo a | (cat || echo b) | cat
// a

// se presente una redir di subshell
// PROBLEMA: non ho questa informazione. non so se la redir è sub_level o cmd related..
// TO FIX!

// supponendo di sapere se redir è sub_level o cmd related

// se è presente anche input redir sub_level: la  redir vince sulla PIPE!!

// bash-3.2$ echo a | (cat || echo b) <test| cat   (test:gg)
// gg

// se aggiungiamo anche input cmd related ( e:ok|test:gg)

// bash-3.2$ echo a | (cat <e || echo b) <test| cat
// ok

// LS $HOME/Desktop

// -------
// CONCLUSIONE: ordine importanza INPUT
// 1: cmd_level
// 2: sub_level
// 3: PIPE

// tuttavia l ordine in cui vanno settate è inverso, ovvero:
// 1: setto PIPE input (vale per solo per primo cmd se subshell)
// 2: setto sub_level input
// 3: setto cmd_level input

// quindi operativamente prima di ogni pipe si esegue la func pipe()

// CONCLUSIONE: ordine importanza OUTPUT
// 1: cmd_level
// 2: sub_level
// 3: PIPE

// tuttavia l ordine in cui vanno settate è inverso, ovvero:
// 1: setto PIPE output (vale per solo per ultimo cmd se subshell) 
// 2: setto sub_level output
// 3: setto cmd_level output


// 1: bash-3.2$ echo a | (echo d &&  echo b) | cat
// d
// b

// 2: bash-3.2$ echo a | (echo d >z &&  echo b) | cat  (z:d)
// b

// bash-3.2$ echo a | (echo d >z &&  echo b) >u | cat  (z:d|u:b)
// bash-3.2$ 

// -------

// N.B:
// - sub_level R_INPUT viene settato SOLO NEL PRIMO CMD!
// - sub_level R_INPUT_HERE_DOC viene settato SOLO NEL PRIMO CMD!
// - sub_level TRUNC_OUTPUT viene settato in TUTTI i CMD!
// - sub_level APPEND_OUTPUT viene settato in TUTTI i CMD!

// N.B:
// bash ha un comportamento anomalo nel caso successivo
// (cat && cat ) <test
// ovvero non richiede here_doc per il secondo comando. sembra non esegua il cmd proprio
// SOLUZIONE: noi facciamo sempre here_doc, come in tutti gli altri casi!
// -------

// execve gestisce da solo l here_doc implicito! [DONE]
// HERE_DOC implicito != here_doc settato, infatti:

// se settato esegue subito here_doc, anche se il comando non viene eseguito
// echop aa && cat <<u

// se implicito non viene eseguito, ma è eseguito solo prima dell esecuzione cmd
// echop aa && cat 

// When the cat command does not contain any arguments, it waits for an input from your keyboard. If you try to run the cat command lacking any arguments, cat will wait for your input from the keyboard until it receives an end-of-file ( EOF ) signal produced by CTRL+D key combination. When entering some input from a keyboard, cat command will simply repeat any input and display it on the screen.


// how to restore FD 
// https://stackoverflow.com/questions/55771495/what-are-the-rules-of-closing-file-descriptors-after-calling-dup-dup2

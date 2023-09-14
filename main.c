/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/25 22:20:06 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/14 16:20:55 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"



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

void addr_matr(char **str, char *msg)
{
    printf("msg:%s|MATR:%p\n", msg, str);
}

void addr_str(char *str, char *msg)
{
    printf("msg:%s|STR:%p\n", msg, str);
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
        if (shell.rawline && shell.rawline[0])
        {
            set_tree(&shell);
            set_components(&shell);
            executeV2(&shell);
            ft_clean_exit(&shell, NULL , 0, 0);
        }
        // ci entra ogni volta che fai invio senza scrivere nulla
    }
}

// risolve il problema dei diversi tipi di skip eventuali,
// in base alle condizioni dell exit status e op del nodo che viene fornito
// tale funzione puo:
// non skippare affatto:
//  - ritorna next_node_same_lvl [DONE]
// skippare:
// - se dopo si presenta un nodo per il quale si dovrebbe forkare una subshell
//   si skippa tutta la subshell e si procede al nodo in cui si dovrebbe rientrare
//   dalla subshell stessa nel padre. [DONE]
// - se si trova dentro una shell o subshell e non bisogna forkare
//    - se il next_cmd ha come back_op and_or, ne skippa solo uno
//    - se il next_cmd ha come back_op pipe, skippa tutta la sequenza
//       fino al prossimo and_or.
// tuttavia se si trova in subshell ed incontra prima la fine della subshell
// fa una clean_exit (o fa fare clean_exit ad executeV2)
// se sta invece in lvl_subshell == 0 invece deve far fare return a executeV2
// riceve come nodo di input quello appena eseguito.
t_node *next_oneV2(t_node *node)
{
    t_node *next;
    t_node *temp;
    t_node *resume;

    next = next_cmd_same_lvl(node);
    temp = node;
    resume = NULL;
    if (norm_checkV2skip(node))
    {
        return (next);
    }
    else
    {
        if (!next)
            return (NULL);
        if (ft_back_node(next)->lvl_subshell > node->shell->lvl_subshell &&
            node->is_last == 0)
        {
            resume = next_cmd2(node->shell, last_cmd_same_lvl(node));
            return (resume);
        }
        if (next->done_lock == 1 || (next && ft_back_node(next)->lvl_subshell < next->shell->lvl_subshell && next->shell->lvl_subshell != 0))
        {
            ft_clean_exit(node->shell, NULL, node->shell->exit_status, 1);
        }
        if (ft_get_op(next) == PIPE)
        {
            return (next);
        }
        if (ft_get_op(next) == AND || ft_get_op(next) == OR)
        {
            resume = next_cmd_same_lvl(next);
            if (!resume)
                return (NULL);
            if (resume->done_lock == 1 || (resume && ft_back_node(resume)->lvl_subshell < resume->shell->lvl_subshell && resume->shell->lvl_subshell != 0))
            {
                ft_clean_exit(node->shell, NULL, node->shell->exit_status, 1);
            }
            return (resume);
        }
    }
    return (NULL);
}

// 5 func

//----------------------------TODO: ----------------------------------
// immagino si puo  togliere can_flag

// uscita ctlr+d exit_status?

// 0.MAIN
// le.s-
// ls *
// echo $?
// fd?
// in wildcard> strjoin2
// 	new_str = ft_strjoin2(new_str, tem); non dovrebbe esse friato new_str

// 1.SIGNAL:
//  a.  void	ft_reset_signal(void) perche?
//  b.  shell->error se è da settare errore, come?



// ------------------- TESTER: OK/KO in base a risultati bash -------------------
// ------------------------ syntax error verificati: OK! ------------------------
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
// echo a <      KO
// (echo a >)    KO
// (echo a >>)   KO
// echo a <<     KO
//  echo a && echo b | echo c ( false && echo d | echo e )          KO
//  echo a && echo b | echo c (&&  false && echo d | echo e )       KO
// echo a && echo b | echo c |( false && echo d | echo e ) echo c   KO

// ---------------------- verificati: OK! ----------------------------------
// ------- >single_cmd<  -------
// echo a>b           OK  out:  (b:a)
// echo ciao >q "r"   OK  out: (q:ciao r)
// echo ciao >q"r"    OK  out: (qr:ciao )
// ("echo')")         OK  out: command not found
// (ls) <g            OK  No such file or directory
// ((ls))             OK  out:ls
// ((ls)>1)>2         OK  out:  (1:ls |2:)
// ((echo a>b) >c)<e  OK  No such file or directory
// echo a >"bb "c>y                 OK  out:  (bb c: y:a)
// echo d >a>b>cc<<ddd              OK  out:  (a: b: cc:d)
// echo a >u>>og <<o"'  pp" p       OK  out:  (og:a p u:)
// ((((echo a)>1)>2)>3)             OK out:   (1:a)
// LS $HOME/Desktop                 OK

// ------- >multi_cmd<  -------
// ------- SENZA SUBSHELL  -------
// a |> b                OK  out: command not found (b:)
// echo a |> b           OK  out:  (b:)
// echo a &&> b          OK  out: a  (b:)
// echo a || < b         OK  out: a  non esegue redir
// echo a && < b         OK  out: a && error_redir
// <<u|echo a            OK  out: a
// echo a|echo b>d<e                               OK  out: redir_error (d:)
// echo a || echo b | cat <<2                      OK  out: a
// echo a || echo b | echo c | echo d && echo e    OK  out: a e ^
// echo a &&echo b | echo c                        OK  out: a c
// echop a && echo b | echo c | echo d && echo e   OK  out: cmd_not_found ^
// echop a && echo b | echo c | echo d || echo e   OK  out: e ^
// io" echo a && echo ba" u                        OK  out: cmd_err
// io " echo a && echo ba" u                       OK  out: cmd_err
// echo a >b || echo b >c                          OK  out: (b:a)
// echo a >b || echo b <<c                         OK  out: (b:a)
// echo ok || echo zi && echo ciao || << gg        OK  out: ok ciao
// -------   CON SUBSHELL  -------
// ((ls) | (ls))                    OK  out:ls
// (echo a && echo b)| cat          OK  out:a b
// (echo a && ls) | cat             OK  out: a + ls
// ( ls  && ls) | cat               OK  out: ls + ls
// (echo b || echo a >e)            OK  out: b
// (echo a && ls)                   OK  out: a + ls
// (echo a &&  cat) >p              OK  out:  (p: a + rimane in attesa ctrl+c)
// (echo a && ls)>2 |cat            OK  out:  (2: a + ls)
// (echo a && echo b)| grep -w a| wc          OK
// (echo a || (echo b ) >u)                   OK out: a
// (echo a || (echo b ) >u)| cat | cat >gg    OK out:    (gg:a)
// (echo a || (echo b ) >u)| cat | cat | grep -w a >gg  OK out: (gg:a)
// (echo a && (echo b && (echo c <z))) >p && echo DDD  OK out: no_such_file (p: a b)
// (echo a && (echo b && echo c)) | cat | grep -w c | cat >dd OK out: (dd:c)
// (echo a || echo b) || echo c && echo r    OK  out: a r
// (echo a || echo b) && echo c && echo r    OK  out: a c r
// (echo a && echo b) && echo c && echo r    OK  out: a b c r
// (echo a && echo b) <m && echo c && echo r OK  out: a b c r (creare m: 1 2) (con o senza m)
// (echo a && echo b) <m && echo c && echo r       OK  out: err_redir m:
// (echo a || (echo b ) >u)                        OK  out: a
// (echo a || (echo b ) <u)                        OK  out: a
// (echo a && (echo b && (echo c <z))) >p| cat     OK  out:err_redir (p: a b)
// ((ls) | echo a && echo c) >zy| cat              OK  out:  (zy: a c)
// ((ls) >zy | echo a && echo c) | cat             OK  out: a c    (zy:ls)
// echo a >a | (echo b >b && echo d >d)            OK  out:        (a:a b:b d:d)
// ((ls) | echo a && echo c) | cat                 OK  out: a c
// (cat | cat | cat  >zi ) <du  && echo d          OK  out: no_such_file
// (cat <zu | cat | cat  >zi ) <du  && echo d      OK: out: no_such_file
// echo a>1|(echo b>2||echo c >3)                  OK  out:  (1:a 2:b)
// echo a>1|(cat>2||echo c  >3)                               OK  out: (1:a 2:)
// echo a && echo b | echo c &&(  false && echo d | echo e )  OK out: a c
// (echo a | echo b && echo c )| echo d                       OK out: d
// echo a | echo b && echo c | echo f                         OK out: b f
// (echo a | echo b | echo c >zi ) >zu && echo d              OK out: d (zi:c zu:)
// (ECHO"b" && (ECHO "a" && (ECHO "d")))|ECHO "c"             OK out: c && cmd_not_found
// echo a >1 && ( echo b >2 || echo c>3) || echo d >4         OK out: (1:a 2:b)
// echo a >1 && ( echo b >2 || echo c <<3) || echo d <<4      OK out: (1:a 2:b) ^
// echo a| (cat ||echo c  >3 ) && echo d<4                    OK out: a && No such file
// echo a | (cat || echo b) | cat                             OK  out: a
// echo a | (echo d &&  echo b) | cat                 OK  out: d b
// echo a | (echo d >z &&  echo b) | cat              OK  out: b   (z:d)
// echo a | (echo d >z &&  echo b) >u | cat           OK  out:     (z:d|u:b)
// (cat | cat | cat  >zi ) <du  && echo d             OK  out:d  (zi: contenuto "du")
// (cat && ls >a) <du  && echo d                      OK: out: d + content "du" (a:ls )
// (cat && ls >a) <du  >u && echo d                   OK: out: d  (a:ls u:content "du" )
// (echo b || (echo a >o))                            OK  out:b
// (echop b && (echo a >o))                           OK  out: cmd_not_found
// (echo b && (echo a ))>i | echo c                   OK  out:c (i:b a)
// (echo b && (echo a )>i) | echo c                   OK  out:c (i:a)
// (echo b && (echo a >o && (echo d))) >p | echo c    OK  out:c (o:a p:b d)
// (echo a && (echo b && (echo c)))| cat              OK  out: a b c
// (echo a >d && (echo b && (echo c)))| cat           OK  out: b c (d: a)
// echop a >b &&  (echo b >c && echo d >e)            OK out: cmd_not_found (b:)
// echo a >b | (echo b >c && (echo d >e | echo f >o)) OK out: (b:a c:b e:d o:f)
// echo a>1|(echo b>2||echo c  >3)                    OK out:   (1:a 2:b)
// echo a && (echo b && (echo c))>d| cat              OK out: a (d:b c)
// echo a >b || (echo b >c && echo d >e)              OK out:  (b: a)
// >input file presenti:(test:gg)
// echo a | (cat || echo b) <test| cat                                OK  out:gg
// >input file presenti: (e:ok|test:gg)
// echo a | (cat <e || echo b) <test| cat                             OK out: ok
// echo a && echo b | (false && echo d | echo e)                      OK out: a
// echo a && echo b | echo c |( echo a && echo d | echo e ) |echo c   OK out: a c
// (echo a && (echo b && echo c && echo d)>d)| cat  OK out:a  (d: b c d)
// (echo a && (echo b && (echo c))>d)| cat           OK  out: a  (d: b c)
// (echo a && (echo b && (echo c <z))) >p            OK  out:  no_such_file  (p:a b)  
// (echo a && (echo b && echo c <u)>p)               OK  out: a + no_such_file (p:b)
// (echo a && (echo b && (echo c))>d)>r | cat        OK out: (d:b c r:a)
// echo d && (echo a && (echo b && echo c <u)>p)>g   OK out: no_such_file (p:b g:a)
// echo d && (echo b && echo c <u)>g                     OK out: d (g:b)
// (echo a && (echo b && (echo c <u))>p) | cat           OK out: a + no_such_file (p:b)
// echo a && echo b | (false && echo d | echo e) >g      OK out: a (g:)
// (echo a && (echo b && (echo c <z))) >p || echo DDD    OK out: DDD + no_such (p: a b)
// (cat | cat | cat  >zi ) <du  || echo d  (du:ciao)     OK  out: d  (zi:ciao)

// -------- 
// (<<u)|echo a    non esegue echo a + esce da lvl0 + dup2_err
// entra in ???? perche setta erroneamente is_last = 1,
// valutare come condizione da aggiungere && cmd != starter_cmd
// (<d | echo b)                    OK  out: err_redir + b
// stesso caso
// echo a >"a" 


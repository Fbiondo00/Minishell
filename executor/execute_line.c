/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/27 22:19:09 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// ---------- EXECUTOR: PARENTHESES & SUBSHELLS ----------
// concettualmente è come se l output dei comandi in subshell viene redirectato
// in un file provvisorio se non esiste oppure in un file indicato fuori tonde
// ma cmq si possono specificare file redir anche internamente le tonde

// ((ls) >zy | echo a && echo c) | cat 

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

// con piu di sublivello di () ?
// EX. ((a | b | u && h |c | d) | (e && f)) && g
//flag:    2   2   2    2  2    1    2         0

// ipotesi esecuzione:
// func per check max livello depth parentheses

// subshell: di fatto "permangono" solo l exit status, ed in caso di command list
// permane un file txt con il risultato parziale.
// che probabilmente va creato pre fork.
// in fase di setup_subshell:
// creo il file o setto quello passato andandomi a fermare nella redirection dell
// node_operator giusto.
// sembra che la max flag si fermi sempre con operatore livello 0
// che poi diventa un livello piu sotto nella subshell in maniera da matchare
// e a scalare le altre flag interne scalano con livello -1


// --------|
// EXECUTOR|
// --------|

// la subshell neanche la fa se fallisce l op  logico ( && || )
// Ex.(echo b || (echo a >o))  /equivalente a/    (echop b && (echo a >o))

// (echo b && (echo a ))>i | echo c    /e' diverso da/ (echo b && (echo a )>i) | echo c
// nella prima dentro i ce':  b a mentre nella seconda c e solo a.
// questo perche il risultato parziale viene perso se non esplicitato(?)


// EX: la sequenza di esecuzione della line:
//  (echo b && (echo a >o && (echo d))) >p | echo c    --->
//          1             2        (??)    0


// difatto cambia il senso della redirection >p che deve diventare append.
// (probabilmente anche il nodo_cmd deve sapere quante subshell deve fare..)

// creo la prima subshell:
// che eseguira' i seguenti comandi:  echo b && echo a >o && (echo d)
// SETTO OUTPUT/INPUT SUBSHELL:
// OUTPUT:
// se non esiste creo file temporaneo con modalita APPEND che salva il risultato parziale delle varie operazioni.
// se esiste file, lo tratto sempre da APPEND. tuttavia da checckare se ha chmod che blocca la modalita append?
// INPUT:
// esempio in generale: fosse cosi, mancherebbe input a cat!:  (cat && echo u <a)   soluzione==> apertura here_doc
// regola generale: se presente input fuori dalle () diventa input del primo cmd della list.
// sempre in generale: (cat <b) <a   ==> vince la redir piu interna.
// nel nostro caso

// N.B per OUTPUT, se presente all esterno viene settato per ogni singolo comando della lista. 
// puo venir sovrascritto dai redir piu interni.

// N.B: di fatto rimane una parentesi, ma non c e'alcun nodo operatore che abbiamo lvl_subshell > 0, si tratta di una 
// parentesi inserita a fine stringa.   Probabilmente puo essere ignorata senza che questo crei problemi di risultato finale.
// anche se alla fine ci fosse stato ... && ((echo d))  -> il risultato sarebbe stato uguale.
// cosi fosse, non dovremmo inserire alcuna flag nei node_cmd.

// prossimo step: capire come il risultato di questa subshell possa tornare indietro, ovvero alla shell precedente.
// domande:
// da che file leggere? che ne sappiamo se e'temp.txt o uno reale?  eventualmente creare flag ad hoc, int is_temp?
// come gestire e comunicare eventuale errore della subshell al livello precedente?
// come fare a sapere se sei in subshell o no? in caso e'possibile usare flag  is_subshell che viene settata prima del fork.


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




//  (echo b && (echo a >o && (echo d))) >p | echo c    --->
//          1             2        (??)    0

// difatto cambia il senso della redirection >p che deve diventare append.
// (probabilmente anche il nodo_cmd deve sapere quante subshell deve fare..)

// creo la prima subshell:
// che eseguira' i seguenti comandi:  echo b && echo a >o && (echo d)
// SETTO OUTPUT/INPUT SUBSHELL:
// OUTPUT:
// se non esiste creo file temporaneo con modalita APPEND che salva il risultato parziale delle varie operazioni.
// se esiste file, lo tratto sempre da APPEND. tuttavia da checckare se ha chmod che blocca la modalita append?
// INPUT:
// esempio in generale: fosse cosi, mancherebbe input a cat!:  (cat && echo u <a)   soluzione==> apertura here_doc
// regola generale: se presente input fuori dalle () diventa input del primo cmd della list.
// sempre in generale: (cat <b) <a   ==> vince la redir piu interna.
// nel nostro caso

// N.B per OUTPUT, se presente all esterno viene settato per ogni singolo comando della lista. 
// puo venir sovrascritto dai redir piu interni.

// N.B: di fatto rimane una parentesi, ma non c e'alcun nodo operatore che abbiamo lvl_subshell > 0, si tratta di una 
// parentesi inserita a fine stringa.   Probabilmente puo essere ignorata senza che questo crei problemi di risultato finale.
// anche se alla fine ci fosse stato ... && ((echo d))  -> il risultato sarebbe stato uguale.
// cosi fosse, non dovremmo inserire alcuna flag nei node_cmd.

// prossimo step: capire come il risultato di questa subshell possa tornare indietro, ovvero alla shell precedente.
// domande:
// da che file leggere? che ne sappiamo se e'temp.txt o uno reale?  eventualmente creare flag ad hoc, int is_temp?
// come gestire e comunicare eventuale errore della subshell al livello precedente?
// come fare a sapere se sei in subshell o no? in caso e'possibile usare flag  is_subshell che viene settata prima del fork.

//  (echo b && (echo a >o && (echo d))) >p | echo c    --->
//          1             2        (??)    0


// SCELTA TECNICA: 
// (( ls  && ls)) | cat
// ((echo a && ls)) | cat

// bash risulta essere inconsistente. nel primo caso non fa nulla(res:newline), quindi non da problemi che ci siano due (())
// tuttavia  nel secondo da errore in quanto sembra non riconoscere echo
// mentre se lanciato cosi: (echo a && ls) | cat   , riconosce echo e da risultato aspettato, ovvero: 
// crea un file temporaneo  e passa i risultati parziali al prossimo cmd
// il redir temp viene settato per tutti i cmd  della subshell, e viene eventualmente sovrascritto dal redir specifico del cmd


// (echo a && (echo b && (echo c))>d)| cat    : res>  a ,  dentro d ce' b c
// (echo a && (echo b && (echo c)))| cat      : res>  a b c
// (echo a && (echo b && (echo c))>d)>r | cat : res> dentro r ce' a, dentro d ce' b c
// (echo a >d && (echo b && (echo c)))| cat   : res> b c , dentro d ce' a

// ---------------  IMPO: EXECUTE()  --------------
// esempio: (echo a && (echo b && echo c && echo d)>d)| cat  ==> su terminale: stampa a / dentro d: stampa b c
//                  1          2          2           0

// si potrebbe pensare questa sequenza:
// creazione subshell ricorsiva, nel senso che:
// 1. guardo next op, essendo lvl_subshell > 0, faccio una subshell, ma faccio a tutti  i cmd di quella list lvl_subshell--;

//         echo a && (echo b && (echo c))>d| cat
//                 0          1            0(non si sa se ha senso fare -1 o lasciare a 0, probabile la prima)

// 2. se non c e general_out, setto redir out di tutti i cmd della lista a temp[i].txt 
// (a prescindere dal fatto che ci sono altre subshell, i e' per unicita'), trovare un modo per stoppare il redir. probabilmente quando 
// trovo altro op che scende di lvl.
// 2.a se nella sub fosse ancora lvl > 0 faccio lvl_subshell-- di tutti i cmd in list e poi un altra subshell e poi setto redir_out temp[i+1].txt
//    se non presente
// 3 essendo il next op con lvl_sub_shell == 0 posso procedere
// 4. controllo se next OP sia | : avrei sovrascritto redir out del cmd
// 5. eseguo echo a (output va in temp.txt) -> eseguire significa sempre un altra subshell / fork...
// 6. guardo prossimo operatore e controllo exit status (faccio il  check waitpid dentro la subshell). ipotizzando exit_success
//    6.a se op fosse stato || : non avrei fatto niente, ovvero avrei fatto semplicemente return
//    6.b essendo op && : vado per eseguire il cmd ma essendo il next op con lvl_subshell ripeto
// l operazione di creazione subshell...ovvero:
// WHILE (lvl_subshell > 0 ):
//      A. faccio tutti i cmd della lista lvl_subshell--;
//      B. faccio subshell
//      C. setto redir general_out/ in
//      D. setto local redir
//      E. eseguo comandi, checckando sempre il next op
//             >se |:  se non presente local redir, sovrascrivo out direction, altrimenti no.(vince local redir)
//             >se || o && : eseguo il cmd 
//                   >il next cmd verra'eseguito o meno in base a exit_status e type op
// 7. incontro next op con lvl_subshell inferiore all attuale.  significa che:
//  HO ESEGUITO TUTTI I CMD DELLA LISTA.
//  tendenzialmente sarebbe da  faccio return ed uscire dalla subshell
//  come e' poi gestione dei temp.txt?? 

// di fatto c e'un if is_parentheses che apre tutto il discorso precedente.
// come start executor si potrebbe tralasciare e passare direttamente alla sequenza di &&, ||, |
// che va a risolvere la parte iniziale sul check dell exit_status e porta avanti il rework redir + parser (lvl_subshell)


// V1: al momento senza parentesi, ovvero subshell, lvl_subshell etc..etc..
// regole sulle redir:

// echo a >b || echo b >c (ma anche >>c)  => non crea c, perche non esegue il comando
// echo a >b || echo b <<c   solo nel caso dell here_doc lo esegue comunque
// echo a >b || (echo b >c && echo d >e) => non crea c & e perche non esegue il cmd

// do_redir(t_node *node)
// 1. vado start node.
// 2. mi giro tutti i nodi ed eseguo tutti gli here_doc (forse anche i cat senza input)
// 3. ritorno allo start node.
// relativamente al singolo nodo fa le seguenti valutazioni:

// PREMESSA:
// 1. echo a >b | (echo b >c && echo d >e) => CREA B C E
//                0            1
// 2. echop a >b &&  (echo b >c && echo d >e) => errore echop + CREA SOLO B
// ovvero di default non fa le redirections
// echo a >b ||  (echo b >c && echo d >e) => CREA B
//    A. esegue tutte le redir se stesso lvl_subshell & pipe, ese

// CONCLUSIONE:
// possiamo dire che vengono eseguite tutte le redir dei cmd legati con pipe,
// ci fermiamo solo se incontriamo gli altri due operatori && ||
// tuttavia se c è parentesi dopo pipe, ovvero se lvl_subshell del successivo
// è maggiore del precedente allora eseguo tutte le redir fino a che non torno nuovamente
// al livello precedente, ovvero il livello del primo cmd
// [FALSO] anche nella subshell si esegue fino a che non trovi && o ||
// quindi se ci sono altri livelli innestati all interno vengono eseguiti. quindi tutti
// fino a che non torno livello di origine

// ovvero caso: echo a >b | (echo b >c && (echo d >e | echo f >o)) => CREA B C E O




// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// 2. executor & subshells

//    B.  PER EXECUTOR lvl_subshell: se termina senza operatore fuori dalle tonde,
//        effettivamente non esiste un nodo_op con lvl 0. quindi si puo
//        considerare il raggiungimento dell ultimo nodo come se ci fosse lo zero
//        esempio creazione is_last_cmd() se necessario

// esegue il singolo cmd, ovvero:
// fa le redir (tranne here_doc che è gia fatta)
// if r_input fail, stampa errore
// altrimenti esegue il cmd
// reset fd_originali
void ft_single_cmd(t_node *node)
{
    int     status;
    pid_t   pid;

    printf("in single_cmd...\n");
    if (!ft_do_redir(node))
    {
        printf("!ft_do_redir\n");
        node->shell->exit_status = 1;
        ft_reset_original_fd(node);
        return;
    }
    // printf("OUT\n");
    if (node->content.cmd && node->content.cmd[0])
    {
        // printf("IN\n");
        if (is_builtin(node))
            execute_builtin(node, node->shell);
        else
        {
            // printf("fork..\n");
            // refactor per utilizzo anche in pipe
            pid = fork();
            if (pid == 0)
                ft_execve(node);
            else
            {
                waitpid(pid, &status, 0);
                node->shell->exit_status = WSTOPSIG(status);
                // printf("node->shell->exit_status:%d\n", node->shell->exit_status);
            }
        }
    }
    ft_reset_original_fd(node);
    // printf("POST: ft_reset_original_fd|pid:%d\n",getpid());
}

void ft_do_subshell(t_node *node)
{
    (void) node;
    // 1. crea file fittizio in append (se non esiste già), se esistono i file fa
    // ft_fd_sub_level del livello in oggetto
    // 2. fork()
    // 3. set std_out ad ogni cmd finchè lvl_subshell (next_cmd) è >0
    //    se invece è <= 0 non settare. break.
    // 4. fare --lvl_subshell a tutti i nodi_cmd che rispettano la condizione
    //    uguale alla precendente. una funzione puo fare entrambe le cose insieme.
    // ft_minus_lvl_subshell(node) ??

    // questa funzione non esegue il cmd, ma prepara solo l environment.

    // variabile pid come attributo a shell??
    // if(node->shell->pid == 0)
        // sono nella shell padre... si forka dalla figlia però?
}


// singola funzione che gestisce sia l or che l and
// all interno una funzione
t_node *ft_do_and_or(t_node* node)
{
    // 1. check status
    // prima di poter eseguire un nodo and o or bisogna controllare exit_status precedente.
    
    if (is_left_branch(node) || ((node->back->content.op == AND && ok_status(node) == 1) ||
        (node->back->content.op == OR && ok_status(node) == 0)))
    {
        ft_single_cmd(node);
        // printf("cmd eseguito, cerco next cmd\n");
        return (next_cmd_same_lvl(node));
    }
    // printf("go_next_cmd_and_or(node)\n");
    // qualora l exit status non permetta l esecuzione del cmd, tale cmd (talvolta insieme ad alcuni altri in caso si tratti delle pipe) vengono skippati.
    // pertanto ritorno il prossimo nodo utile per l esecuzione.
    node->done_lock = 1;
    printf("IN AND_OR:go_next_cmd_and_or\n\n");
    return (go_next_cmd_and_or(node));
}

// 1. pipe
// 2. fork
// 3. check posizione in ipotetica sequenza di pipe.
// n.b: pipe vince su finto append
t_node *ft_do_pipe(t_node* node)
{
    // refactor simple_cmd  per utilizzarlo ....solo la parte dell !is_builtin va
    // reso variabile
    pid_t pid;
    int status;
    int len;
    int fd[2];
    
    len = ft_strlen(node->content.cmd[0]);
    if (!ft_strncmp(node->content.cmd[0], "exit", len, 1))
    { // fork in tutti i casi tranne exit.
        printf("eseguo exit...\n");
        if (!ft_do_redir(node))
        {
            printf("perche qui!\n");
            node->shell->exit_status = 1;
            return (next_cmd_same_lvl(node));
        }
        execute_builtin(node, node->shell);
    }
    else
    {
        // se è ultimo elemento sequenza pipe, ristabilire stdout
        // e non fare pipe()
        if (next_cmd_same_lvl(node) && next_cmd_same_lvl(node)->back->content.op == PIPE)
        {
            if (pipe(fd) == -1)
                perror("pipe");
        }
        pid = fork();
        if (pid == 0)
        {
            if (next_cmd_same_lvl(node) && next_cmd_same_lvl(node)->back->content.op == PIPE && next_cmd_same_lvl(node)->done_lock != 1)
            {
                printf("FINISCE QUI!\n");
                // print_node(node->shell, node);
                close(fd[0]);
                ft_dup2(&fd[1], STDOUT_FILENO);
            }
            else
                dup2(node->shell->temp_output, STDOUT_FILENO);
            if (!ft_do_redir(node))
                exit(33);
            if (is_builtin(node))
            {
                execute_builtin(node, node->shell);
                exit(0);
            }
            else
                ft_execve(node);
        }
        else
        {
            if (next_cmd_same_lvl(node) &&  next_cmd_same_lvl(node)->back->content.op == PIPE)
            {
                close(fd[1]);
                ft_dup2(&fd[0], STDIN_FILENO);
            }
            else
                dup2(node->shell->temp_output, STDOUT_FILENO);
            waitpid(pid, &status, 0);
            node->shell->exit_status = WSTOPSIG(status);
            printf("node->shell->exit_status:%d\n", node->shell->exit_status);
        }
    }
    return(next_cmd_same_lvl(node));
}


// LOGICA FD != LOGICA ()
// LOGICA FD == LOGICA OP
// echo a>1|(echo b>2||echo c  >3) |out:  |1:a |2:b  // il file 3 non viene creato
// non viene neanche eseguito. però si va in subshell, in quanto il cmd prima va eseguito.
// quindi: la logica di priorità dei fd non è influenzata dalla logica delle parentesi
// ma segue la logica degli operatori

// a livello operativo corrisponde a fare la redir solo prima dell esecuzione del suo cmd
// e non in blocco insieme alle redir di altri cmd

void execute(t_shell *shell)
{
    t_node *node;
    t_node *next_node;

    printf("------------------|FASE: EXECUTOR|------------------\n");
    //  fa tutti gli open degli here_doc settati in s_kv, non fa il dup2
    //  echo a || echo b | cat <<2
    if (is_node_cmd(shell->tree))
    {
        printf("nodo op è assente...\n");
        ft_do_heredoc(shell->tree);
        ft_single_cmd(shell->tree);
        return ;
    }
    printf(" è presente almeno 1 nodo op...\n");
    node = go_to_starter_node(shell->tree->left);
    ft_do_heredoc(node); // loop
    // V1: multi_cmd_without parantheses
    // tutti i nodi sullo stesso livello.
    while (1)
    {
        if (!node || node->done_lock == 1)
        {
            if (!node)
                ft_reset_original_fd(shell->tree);
            if (node && node->done_lock == 1)
                ft_reset_original_fd(node);
            break;
        }
        if (node->back && node->back->content.op == PIPE)
        {
            printf("ft_do_pipe...\n");
            if (node->content.cmd && node->content.cmd[0])
                next_node = ft_do_pipe(node);
            else
            {
                printf("PIPE && ONLY REDIR\n");
                if (!ft_do_redir(node))
                {
                    node->shell->exit_status = 1;
                    ft_reset_original_fd(node); 
                }
                else
                    node->shell->exit_status = 0;
                next_node = next_cmd_same_lvl(node);
            }
            printf("PIPE|next_node:p%p\n", next_node);
        }
        else if (node->back && (node->back->content.op == AND || node->back->content.op == OR))
        {
            printf("ft_do_and_or...\n");
            next_node = ft_do_and_or(node);
            printf("AND_OR|next_node:p%p\n", next_node);
        }
        node = next_node;
    }
    // in generale per l ultimo cmd, se non op dopo? come esco da subshell ?
        // // questa funzione non esegue il cmd, ma prepara solo l environment.
        // if (node->back && node->back->lvl_subshell > 0)
        //     ft_do_subshell(node);    
}

// 5 func


// subshell + hidden here_doc
// syntax error
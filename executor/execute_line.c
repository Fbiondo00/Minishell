/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/30 05:20:52 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void execute_recursive(t_node *node);

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


int norm_check(t_node *node)
{
    if ((node->back->content.op == AND && ok_status(node) == 1) ||
        (node->back->content.op == OR && ok_status(node) == 0))
        return (1);
    else
        return (0);
}

// singola funzione che gestisce sia l or che l and
// all interno una funzione
t_node *ft_do_and_or(t_node* node, t_node *prev_node)
{
    // 1. check status
    // prima di poter eseguire un nodo and o or bisogna controllare exit_status precedente.
    printf("norm_check(node):%d\n", norm_check(node));
    if (!prev_node  || (prev_node && norm_check(node)))
    {
        ft_single_cmd(node);
        printf("cmd and_or eseguito....\n");
        if (norm_check(node) && next_cmd_same_lvl(node) && next_cmd_same_lvl(node)->back->content.op == PIPE)
        {
            printf("HERE!\n");
            return (next_cmd_same_lvl(node));
        }  
        else
        {
            printf("return:go_next_cmd_and_or(node)...\n");
            return (go_next_cmd_and_or(node)); // se non devo entrare nella sequenza non ci entro
        }
    }
    // qualora l exit status non permetta l esecuzione del cmd, tale cmd (talvolta insieme ad alcuni altri in caso si tratti delle pipe) vengono skippati.
    // pertanto ritorno il prossimo nodo utile per l esecuzione.
    node->done_lock = 1;
    printf("IN AND_OR:go_next_cmd_and_or\n\n");
    return (go_next_cmd_and_or(node));
}

// ritorna 1 se è ultimo  node della pipe-chain
int is_last_pipe(t_node *node)
{
    if (next_cmd_same_lvl(node) && next_cmd_same_lvl(node)->back->content.op == PIPE)
        return (0);
    return (1);
}


// n.b1: pipe vince su finto append
// n.b2:refactor simple_cmd  per utilizzarlo 
// ....solo la parte dell !is_builtin vareso variabile
t_node *ft_do_pipe(t_node* node)
{
    
    pid_t pid;
    int status;
    int len;
    int fd[2];
    
    len = ft_strlen(node->content.cmd[0]);
    if (!ft_strncmp(node->content.cmd[0], "exit", len, 1))
    { // fork in tutti i casi tranne exit.
        printf("eseguo exit...\n");
        if (ft_do_redir(node) == 0)
        {
            printf("errore durante redir!\n");
            return (next_cmd_same_lvl(node));
        }
        execute_builtin(node, node->shell); // poi va a return next_cmd_same_lvl
    }
    else
    {
        // se è ultimo elemento sequenza pipe, ristabilire stdout e non fare pipe()
        if (!is_last_pipe(node))
        {
            if (pipe(fd) == -1)
                perror("pipe");
        }
        pid = fork();
        if (pid == 0)
        {
            if (!is_last_pipe(node))// && next_cmd_same_lvl(node)->done_lock != 1)
            {
                close(fd[0]);
                ft_dup2(&fd[1], STDOUT_FILENO);
            }
            else
                dup2(node->shell->temp_output, STDOUT_FILENO);
            if (ft_do_redir(node) == 0)
                exit(33);
            if (is_builtin(node))
                exit(execute_builtin(node, node->shell));
            else
                ft_execve(node);
        }
        else
        {
            if (!is_last_pipe(node))
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




 // cercando sempre tra tutti i nodi tra tutti i nodi fino a che 
 // ce ne possono essere piu di una
// int count_redir_sub_err(node)
// {
//     // cerca in tutte le redir se è presente una di err, del livello giusto
// }

// int init_shell_redir(t_node *node, char ***shell_redir)
// {
//     int count;

//     count = 0; // conta di default su_out, che deve sempre esserci 
//     count += count_redir_sub_err(node);
//     count += count_redir_sub_in(node);
//     if (count_redir_sub_out(node) == 0)
//         count++;
//     else
//         count += count_redir_sub_out(node);
//     *shell_redir = malloc (count);
//     settare null byte...
//     return (count);
// }

// va eseguita qua ft_fd_sub_level, perche non vale solo per il singolo cmd
// ma per tutto il suo "gruppo"

// nuovo elemento in shell, struct t_kv che contiene tutte le redir >1, o in alternativa
// un array di stringhe contenente solo i nomi dei file, dovremo poi eliminarli tutti,
// uno per uno

// questa funzione non esegue il cmd, ma prepara solo l environment.
void ft_do_subshell(t_node *node)
{
    // int count;
    // char **shell_redir;
    
    // malloca la matr con il numero esatto di redir da fare
    // count = init_shell_redir(shell_redir);
    // cercando sempre tra tutti i nodi tra tutti i nodi fino a che 
    // node->back->lvl_subshell è > 0, oppure si puo dire finche è > di lvl_subshell
    // della shell

    //count =count_redir_sub_err(node)
    // while (count > 0)

    
    (void)node;
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


// ritorna l ultimo cmd appartenente allo stesso lvl
t_node *last_cmd_same_lvl(t_node *node)
{
    (void) node;
    return (NULL);
}

void set_shell_env(t_node *node, int lvl_subshell, char **arredir)
{
    (void)node;
    (void)lvl_subshell;
    (void)arredir;
}

// fa il close ed il free, set a NULL.. simile a ft_remove_heredoc
void ft_free_hidden_redir(char **arredir)
{
    (void)arredir;
}

t_node *fork_execute(t_node *node, char **arredir)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        // setta tutte le sub_redir
        // se arredir !NULL, la setta come input
        // se deve creare un hidden redir la salva nella stringa salva nella new_arredir
        // fa shell->lvl_subshell++
        set_shell_env(node, node->lvl_subshell, arredir);
        execute_recursive(node);
    }
    else
    {
        // siamo nel padre
        waitpid(pid, &status, 0);
        node->shell->exit_status = WSTOPSIG(status);
        if (arredir) // fa il close ed il free, set a NULL.. simile a ft_remove_heredoc
            ft_free_hidden_redir(arredir);
    }
    return (next_cmd2(node->shell, last_cmd_same_lvl(node)));
}


// coincide con la parte del multi_cmd dell attuale execute
// esegue tutta la sequenza di nodi_cmd del suo stesso lvl
// si blocca se finisce la riga
t_node *just_execute(t_node *node)
{
    (void)node;
    return (NULL);
}

void execute_recursive(t_node *node)
{
    char *arredir;
    t_node *next_node;

    while (1)
    {   // questo è il caso in cui è finita la stringa
        // se !node, puo essere anche che non abbiamo ancora chiuso le subshell..
        // quindi check se si è ancora in subshell.. controllando se lvl_subshell > 0
        if (!node || node->done_lock == 1) // done_lock ??
            return (ft_reset_original_fd(node->shell->tree));
        if (node->back->lvl_subshell == node->shell->lvl_subshell)
            next_node = just_execute(node);
        else if (node->back->lvl_subshell > node->shell->lvl_subshell)
            next_node = fork_execute(node, &arredir);
        else if (node->back->lvl_subshell < node->shell->lvl_subshell)
        {
            //caso in cui è finita subshell
            ft_clean_exit(node->shell, NULL, node->shell->exit_status, 1);
        }
        node = next_node;
    }
}

// coincide con la parte iniziale dell attuale execute
void execute_single_cmd(t_node *node)
{
    (void)node;
}

// main
//  execute(ft_starter(shell));
// riceve lo starter node
// node = ft_starter(shell);
void execute2(t_shell *shell)
{
    // simile a quello attuale solo che fa anche le redir_sub_lvl
    // nell ordine corretto
    if(is_node_cmd(shell->tree))
        execute_single_cmd(shell->tree);
    else
        execute_recursive(go_to_starter_node(shell->tree->left));
}

void execute(t_shell *shell)
{
    t_node *node;
    t_node *next_node;
    t_node *prev_node; //backlog and_or logic

    prev_node = NULL;
    printf("------------------|FASE: EXECUTOR|------------------\n");
    if (is_node_cmd(shell->tree))
    {
        printf("nodo op è assente...\n");
        ft_do_heredoc(shell->tree);
        ft_single_cmd(shell->tree);
        return ;
    }
    printf(" è presente almeno 1 nodo op...\n");
    node = go_to_starter_node(shell->tree->left);
    ft_do_heredoc(node);
    while (1)
    {
        // if (node->back->lvl_subshell > 0) // non va messo come primo: seg fault
        //     ft_do_subshell(node);
        if (!node || node->done_lock == 1)
        {
            if (node)
                printf("node->done_lock:%d|esce da execute...\n", node->done_lock);
            ft_reset_original_fd(shell->tree);
            return ;
        }
        else if (!node->content.cmd && !node->content.cmd[0])
        {
            printf("NODE_CMD WITH ONLY REDIR...\n");
            ft_do_redir(node);
            printf("PRE:JUST_REDIR|node:p%p\n", node);
            printf("PRE:JUST_REDIR|prev_node:p%p\n", prev_node);
            prev_node = node;
            next_node = next_cmd_same_lvl(node);
            printf("POST:JUST_REDIR|prev_node:p%p\n", prev_node);
            printf("POST:JUST_REDIR|next_node:p%p\n", next_node);
        }
         if (node->back->content.op == PIPE)
        {
            printf("ft_do_pipe...\n");
            printf("PRE:PIPE|prev_node:p%p\n", prev_node);
            next_node = ft_do_pipe(node);
            prev_node = node;
            printf("PRE:PIPE|node:p%p\n", node);
            // next_node = next_cmd_same_lvl(node);
            printf("POST:PIPE|next_node:p%p\n", next_node);
            printf("POST:PIPE|prev_node:p%p\n", prev_node);
        }
        else if (node->back->content.op == AND || node->back->content.op == OR)
        {
            printf("ft_do_and_or...\n");
            printf("PRE:AND_OR|node:p%p\n", node);
            printf("PRE:AND_OR|prev_node:p%p\n", prev_node);
            next_node = ft_do_and_or(node, prev_node);
            prev_node = node;
            printf("AFTER:AND_OR|next_node:p%p\n", next_node);
            printf("POST:AND_OR|prev_node:p%p\n", prev_node);
        }
        node = next_node;
    }
}

// 5 func
// subshell + hidden here_doc



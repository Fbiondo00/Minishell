/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/09 17:48:50 by flaviobiond      ###   ########.fr       */
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



// -------------|
// REWORK PARSER|
// -------------|

// nuovo attributo da settare in node_operator(non node_cmd):   int lvl_subshell;
// logica:
// 1. parte da starter node (node_cmd):
//      - se incontra il char ( ,o anche piu di uno,  fa lvl_subshell++ sul node_operator successivo.
//      - se incontra il char ) ,o anche piu di uno,  fa lvl_subshell-- sul node_operator successivo.
//      - se incontra sia il char ( sia il char ) fa la differenza ed eventualmente  fa lvl_subshell++/-- sul node_operator successivo.
//      - se non incontra nessuno dei due char eredita il lvl_subshell del node_operator precedente.
// risale tutto l albero fino ad arrivare all ultimo nodo.

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

*/


void print_node(t_node *node)
{
    int i;

    i = -1;
    printf("----EXECUTE PHASE----\n\n");
    printf("Start node address:%p\n", node);
    while (node->content.cmd[++i])
        printf("Start node cmd[%d]:%s\n", i, node->content.cmd[i]);
    printf("node->raw_cmd:%s\n", node->raw_cmd);
    //dove e'presente info delle (??
}


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
// esempio: (echo a && (echo b && (echo c))>d)| cat  ==> su terminale: stampa a / dentro d: stampa b c
//                  1          2              0

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


void    execute(t_shell *shell)
{
    t_node *node;

    printf("Shell->tree address:%p\n", shell->tree);
    printf("Calculating starting node...\n");
    node = get_starter_node(shell);
    print_node(node);
    // attualmente il parser non ha tolto le parentesi
}




// ------------------ INIZIO CODICE FBIONDO --------------------------
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
    else if (!ft_strncmp(node->content.cmd[0], "export", len))
        ft_export(shell, node);
    else if (!ft_strncmp(node->content.cmd[0], "unset", len))
         ft_unset(node, shell);
    // else if (!ft_strncmp(node->content.cmd[0], "exit", len))
    //     // ft_exit(node);
    else if (!ft_strncmp(node->content.cmd[0], "cd", len))
        ft_cd(node, shell);
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
    return (0);
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
    // exit(99);
}
// ------------------ FINE CODICE FBIONDO --------------------------
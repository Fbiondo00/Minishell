/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 23:32:44 by rdolzi            #+#    #+#             */
/*   Updated: 2023/09/02 05:07:08 by rdolzi           ###   ########.fr       */
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

// EX. (a | b | u && h |c | d) | (e && f)
// flag:  1   1   1    1  1     0    1

// una volta configurata, quando andremo a cercare i comandi da inserire il lista
// partendo dallo starter node andremo a verificare la flag.
// in caso stia ad 1 verifico se il nodo_operatore precedente stia sempre ad 1 e cosi via.
// quando troviamo il primo 0, raggruppiamo tutti i nodi fino a quel momento sotto un unica
// subshell.

// con piu di sublivello di () ?
// EX. ((a | b | u && h |c | d) | (e && f)) && g
// flag:    2   2   2    2  2    1    2         0

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

// LOGICA FD != LOGICA ()
// LOGICA FD == LOGICA OP
// echo a>1|(echo b>2||echo c  >3) |out:  |1:a |2:b  // il file 3 non viene creato
// non viene neanche eseguito. però si va in subshell, in quanto il cmd prima va eseguito.
// quindi: la logica di priorità dei fd non è influenzata dalla logica delle parentesi
// ma segue la logica degli operatori
// a livello operativo corrisponde a fare la redir solo prima dell esecuzione del suo cmd
// e non in blocco insieme alle redir di altri cmd

int norm_check(t_node *node)
{
    if ((node->back->content.op == AND && ok_status(node) == 1) ||
        (node->back->content.op == OR && ok_status(node) == 0))
        return (1);
    else
        return (0);
}

// ritorna 1 se è ultimo  node della pipe-chain
int is_last_pipe(t_node *node)
{
    if (next_cmd_same_lvl(node) && next_cmd_same_lvl(node)->back->content.op == PIPE)
        return (0);
    return (1);
}

char *sign_str(t_node *node, int lvl)
{
    char *str;
    char *alf;
    (void)node;

    alf = "ABCDEFGHILMNOPQRSTUVZabcdefghilmnopqrstuvz9876543210";
    printf("vale of lvl: %d\n", lvl);
    str = ft_strjoin("lvl", alf + ft_strlen(alf) - 1 - lvl);
    printf("create_signature:%s|len:%d\n", str, ft_strlen(str));
    return (str);
}

// ritorna l ultimo cmd appartenente allo stesso lvl
// se non esiste un next_cmd ritorna il nodo di input
// di fatto non torna mai NULL
t_node *last_cmd_same_lvl(t_node *node)
{
    t_node *temp;
    t_node *next_node;

    temp = node;
    next_node = NULL;
    while (1)
    {
        next_node = next_cmd_same_lvl(temp);
        if (!next_node)
            return (temp);
        temp = next_node;
    }
    return (NULL);
}

// ritorna 1 se nel node storage  indicato è presente
int is_redir_out(t_node *node, int lvl)
{
    int i;

    i = -1;
    while (++i < node->content.kv_size)
    {
        if (node->content.redir[i].lvl == lvl && (node->content.redir[i].key == R_OUTPUT_APPEND || node->content.redir[i].key == R_OUTPUT_TRUNC))
            return (1);
    }
    return (0);
}

// 5 func
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/19 16:53:44 by rdolzi           ###   ########.fr       */
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

// non utilizzata attualmente
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



// ------------------ INIZIO CODICE FBIONDO --------------------------
// attenzione al subject:
// a volte specifica senza options altre volte senza arguments.
// come gestire i vari casi?
// definire i metodi nei rispettivi file..
void execute_builtin(t_node *node, t_shell *shell)
{
    int len;

    len = ft_strlen(node->content.cmd[0]);
    if (!ft_strncmp(node->content.cmd[0], "echo", len, 1))
        ft_echo(node);
    if (!ft_strncmp(node->content.cmd[0], "pwd", len, 1))
        ft_pwd();
    else if (!ft_strncmp(node->content.cmd[0], "env", len, 1))
        ft_env(shell);
    else if (!ft_strncmp(node->content.cmd[0], "export", len, 1))
    {
        printf("prima di ft_export..\n");
        ft_export(shell, node);
    }
    else if (!ft_strncmp(node->content.cmd[0], "unset", len, 1))
        ft_unset(node, shell);
    else if (!ft_strncmp(node->content.cmd[0], "exit", len, 1))
        ft_exit(node, shell);
    else if (!ft_strncmp(node->content.cmd[0], "cd", len, 1))
        ft_cd(node, shell);
}

// ritorna 1 se è un comando builtin
// ritorna 0 in caso contrario
int is_builtin(t_node *node)
{
    int len;

    len = ft_strlen(node->content.cmd[0]);
    if (!ft_strncmp(node->content.cmd[0], "echo", len, 1))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "pwd", len, 1))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "env", len, 1))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "export", len, 1))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "unset", len, 1))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "exit", len, 1))
        return (1);
    else if (!ft_strncmp(node->content.cmd[0], "cd", len, 1))
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
        printf("Non è builtin!\n");
    // exit(99);
}
// ------------------ FINE CODICE FBIONDO ----------------------------------

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// 2. executor & subshells

//    A.  cat senza input fare here_doc!(di default oppure a seconda di op logici?)
// here_doc implicito del cat non viene eseguito a prescindere ma durante exec
// del proprio cmd, pertanto puo anche non essere mai eseguito, (se: && ||)
// ex. echo a || cat

//    B.  PER EXECUTOR lvl_subshell: se termina senza operatore fuori dalle tonde,
//        effettivamente non esiste un nodo_op con lvl 0. quindi si puo
//        considerare il raggiungimento dell ultimo nodo come se ci fosse lo zero
//        esempio creazione is_last_cmd() se necessario


// -------------------- fd.c / execve.c / open_file.c --------------------

// se il cmd[0] fa parte di un set di comandi che richiedono un input esegue un
// here_doc implicito. tale here_doc viene bloccato solo da signal ctrl+D
// un check tipo is_builtin
// void ft_do_implicit_heredoc(t_shell *node)
// {
//     (void) node;
// }

// si scorre tutto l albero ed esegue tutti gli here_doc settati +
// void ft_do_heredoc(t_shell *shell)
// {
//     (void) shell;
// }

// reset fd originali(con dup2), salvati in shell
// non facciamo chiusura dei dup temp_.. perche servono per il comandi successivi.
// possibile che questa funzione vada usata quando incontriamo AND e OR, PIPE
void ft_reset_original_fd(t_node *node)
{
    dup2(node->shell->temp_input, STDIN_FILENO);
    dup2(node->shell->temp_output, STDOUT_FILENO);
    dup2(node->shell->temp_error, STDERR_FILENO);
}

int ft_dup2(int *fd, int arg)
{
    if (dup2(*fd, arg) == -1)
    {
        perror("Dup2 error");
        return (0);
    }
    close(*fd);
    return (1);
}

int	ft_here_doc(int *fd, char *limiter)
{
	char	*str;

    str = NULL;
	*fd = open("temp.txt", O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if (*fd == -1)
        return (0);
	while ((ft_strncmp(str, limiter, ft_strlen(str) - 1, 0)) || (
			ft_strlen(str) - 1) != ft_strlen(limiter))
	{
		write(1, &">", 1);
		str = get_next_line(0);
		if ((ft_strncmp(str, limiter, ft_strlen(str) - 1, 0)) && write(
				*fd, str, ft_strlen(str)) == -1)
			perror("Write error");
		if (!str)
		{
			if (unlink("./temp.txt") != 0)
				perror("unlink error");
		}
		free(str);
	}
    close(*fd);
    *fd = open("temp.txt", O_RDONLY, 0777);
    return (1);
}

int ft_open(int *fd, char *str, int key)
{
    if (key == R_OUTPUT_APPEND)
    {
        // printf("FT_OPEN:key == R_OUTPUT_APPEND\n");
        *fd = open(str, O_RDWR | O_CREAT | O_APPEND | O_CLOEXEC, 0777);
    }
    else if (key == R_OUTPUT_TRUNC)
    {
        // printf("FT_OPEN:key == R_OUTPUT_TRUNC\n");
        *fd = open(str, O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0777);
    }
    else if (key == R_INPUT)
    {
        // printf("FT_OPEN:key == R_INPUT\n");
        *fd = open(str, O_RDONLY | O_CLOEXEC, 0777);
    }
    if (*fd == -1)
        return (0);
    // printf("ft_open: FILE APERTO!\n");
    return (1);
    }

// int type > node->content.redir[i].key
// int fd > node->content.redir[i].fd
// N.B: per far vincere l ultimo continuare con gli n-open, salvarli sempre in node->std..
//      se presenti successivi chiuderle il precedente prima di apertura nuovo
// in realtà è possibile che non serva utilizzare questi attributi!
// gia avviene la pulizia corretta degli fd.
void ft_clean_prev_fd(t_node *node, int type, int fd)
{
    if ( node->std_in != -1  && (type == R_INPUT || type == R_INPUT_HERE_DOC))
        close(node->std_in);
    else if ( node->std_out != -1  && fd == 1 && (type == R_OUTPUT_APPEND || type == R_OUTPUT_TRUNC))
        close(node->std_out);
    else if (node->std_out != -1 && fd == 2 && (type == R_OUTPUT_APPEND || type == R_OUTPUT_TRUNC))
        close(node->std_err);
}

// esegue le seguenti operazioni sul redir[idx]
// open file
// se ok -> fa il dup2, con l fd relativo (se non input o se non -1)
// se ok -> ritorna null
// ritorna 1 se ok, 0 se errore
//-9. chiudere here_doc dopo utilizzo ?? salvare variabile in struct?

int ft_open_file(t_node *node, int i)
{
    int fd;

    if (node->content.redir[i].key == R_INPUT_HERE_DOC)
        ft_here_doc(&fd, node->content.redir[i].value);
    else
        ft_open(&fd, node->content.redir[i].value, node->content.redir[i].key);
    if (fd == -1)
    {
        perror("Open error");
        return (0);
    }
    // ft_clean_prev_fd(node, node->content.redir[i].key, node->content.redir[i].fd);
    if (node->content.redir[i].fd != -1 && (node->content.redir[i].key == R_OUTPUT_TRUNC || node->content.redir[i].key == R_OUTPUT_APPEND))
    {
        // printf("node->content.redir[i].fd != -1 && KEY:R_OUTPUT_TRUNC|| R_OUTPUT_APPEND");
        // printf("ft_dup2(&fd:%d, node->content.redir[i].fd:%d)\n", fd, node->content.redir[i].fd);
        ft_dup2(&fd, node->content.redir[i].fd);
    }
    else if (node->content.redir[i].key == R_INPUT || node->content.redir[i].key == R_INPUT_HERE_DOC)
    {
        // printf("KEY:R_INPUT|| R_INPUT_HERE_DOC");
        // printf("ft_dup2(&fd:%d, node->content.redir[i].fd:%d)\n", fd, STDIN_FILENO);
        ft_dup2(&fd, 0);
    }
    else if (node->content.redir[i].key == R_OUTPUT_TRUNC || node->content.redir[i].key == R_OUTPUT_APPEND)
    {
        // printf("KEY:R_OUTPUT_TRUNC|| R_OUTPUT_APPEND");
        // printf("ft_dup2(&fd:%d, node->content.redir[i].fd:%d)\n", fd, STDOUT_FILENO);
        ft_dup2(&fd, 1);
    }
    return (1);
}

// esegue la redir del lvl indicato.
// nel caso si trattasse di redir di input, o anche here_doc(?)
// esegue solo se is_first == 1
// se si tratta di output esegue sempre.
// ritorna 1 se ok, 0 se errore
int ft_fd_sub_level(t_node *node, int lvl, int is_first)
{
    int i;

    i = -1;
    printf("in ft_fd_sub_level...\n");
    while (++i < node->content.kv_size)
    {
        printf("A\n");
       if (node->content.redir[i].lvl == lvl && (is_first && (node->content.redir[i].key == R_INPUT || node->content.redir[i].key == R_INPUT_HERE_DOC)))
       {
            printf("ft_fd_sub_level: FAft_open_file!\n");
            if(!ft_open_file(node, i))
                return (0);
        }
    }
    return (1);
}

// esegue tutte le redir con lvl 0(cmd_level)
// ritorna 1 se ok, 0 se errore
// check if (node->content.redir)
int  ft_fd_cmd_level(t_node *node)
{
    int i;

    i =-1;
    printf("in ft_fd_cmd_level...\n");
    while (++i < node->content.kv_size)
    {
         if (node->content.redir[i].lvl == 0)
         {
            // printf("ft_fd_cmd_level: FAft_open_file!\n");
            if (!ft_open_file(node, i))
                return (0);
         }
    }
    return (1);
}

void free_matrix(char **matrix)
{
    int i;

    i = -1;
    while (matrix[++i])
        free(matrix[i]);
    free(matrix);
}

char *get_path(char *cmd, char **env)
{
    int i;
    char *temp;
    char *path;
    char **base;

    i = 0;
    // var_expand(node, "PATH") in alternativa al while 
    while (ft_strncmp(env[i], "PATH=", 5, 0))
        i++;
    base = ft_split((env[i] + 5), ':');
    i = -1;
    while (base && base[++i])
    {
        temp = ft_strjoin(base[i], "/");
        path = ft_strjoin(temp, cmd);
        free(temp);
        if (access(path, X_OK) == 0)
        {
            free_matrix(base);
            return (path);
        }
        free(path);
    }
    free_matrix(base);
    return (NULL);
}

void    ft_execve(t_node *node)
{
    char *path;
    
    if (node->content.cmd[0][0] != '/')
        path = get_path(node->content.cmd[0], node->shell->env);
    else
        path = node->content.cmd[0];
    if (!path)
    {
        write(2,"Error: command not found\n", 25);
        exit(20);
    }
    // passare il terzo param, ovvero l env fittizio??
    if (execve(path, node->content.cmd, node->shell->env) == -1) 
    {
        perror("Command Error");
        exit(30);
    }
}

// esegue il singolo cmd, ovvero:
// fa le redir (tranne here_doc che è gia fatta)
// if r_input fail, stampa errore
// altrimenti esegue il cmd
// reset fd_originali
void ft_single_cmd(t_node *node)
{
    pid_t   pid;
    int     max_lvl;

    printf("in ft_single_cmd...\n");
    if (node->content.redir)
        max_lvl = node->content.redir[node->content.kv_size - 1].lvl;
    else
        max_lvl = 0;
    printf("max_lvl:%d\n", max_lvl);
    // fa here_doc impliciti se necessario
    // ovvero se non presenti altri r_input e se cmd corretto
    // ft_implicit_heredoc(node);
    // esegue tutti gli fd cmd_level in ordine decrescente, non esegue lo zero
    while (max_lvl > 0)
    {
        printf("max_lvl:%d\ns", max_lvl);
        if (!ft_fd_sub_level(node, max_lvl--, 0))
            return ;
    }
    printf("PRE: ft_fd_cmd_level(node)\n");
    if (!ft_fd_cmd_level(node))
        return ;
    if (is_builtin(node))
    {
        // printf("eseguo comando builtin...\n");
        execute_builtin(node, node->shell);
    }
    else
    {
        pid = fork();
        // printf("Non è un comando builtin!\n");
        if (pid == 0)
        {
            // printf("nel processo figlio!\n");
            ft_execve(node);
        }
        else
        {
            // eventualmente catturare gli exit_status, se serve
            waitpid(pid, NULL, 0);
        }
    }

    ft_reset_original_fd(node);
    printf("POST: ft_reset_original_fd|pid:%d\n",getpid());
}

// LOGICA FD != LOGICA ()
// LOGICA FD == LOGICA OP
// echo a>1|(echo b>2||echo c  >3) |out:  |1:a |2:b  // il file 3 non viene creato
// non viene neanche eseguito. però si va in subshell, in quanto il cmd prima va eseguito.
// quindi: la logica di priorità dei fd non è influenzata dalla logica delle parentesi
// ma segue la logica degli operatori

// a livello operativo corrisponde a fare la redir solo prima dell esecuzione del suo cmd
// e non in blocco insieme alle redir di altri cmd

// se fallisce calcolo redir input, non viene eseguito il cmd
// EX. echo a <1
// bash: 1: No such file or directory

// si puo risolvere prima single_cmd , poi senza (), poi con () ...
void execute(t_shell *shell)
{
    // t_node *node;

    // esegue tutti gli here_doc settati in s_kv
    // ft_do_heredoc(shell);
    if (is_node_cmd(shell->tree))
    {
        // fare funz
        shell->tree->std_in = shell->tree->shell->temp_input;
        shell->tree->std_out = shell->tree->shell->temp_output;
        shell->tree->std_err = shell->tree->shell->temp_error;
        // --
        printf("nodo op è assente...\n");
        ft_single_cmd(shell->tree);
        // esce sia se ha riscontrato errore che no. In ogni caso fare
        // ft_clean_exit a cui seguirà una newline.
        return ;
    }
    // qui di seguito è presente almeno 1 nodo op
    printf("nodo op è presente...\n");
    printf("PRIMO LOOP...\n");
    // node = go_to_starter_node(shell->tree->left);
    // a livello operativo corrisponde fare la redir solo prima dell esecuzione del suo cmd
    // se fallisce r_input dare errore: No such file or directory  (senza exec cmd)
    // while (1)
    // {
        
    //     if (is_left_branch(node))
    //     {
    //     }
    //     if (!is_left_branch(node) && !node->back->back->lvl_lock)
    //     {
    //     }
    //     else if (!is_left_branch(node) && node->back->back->lvl_lock)
    //     {
    //     }
    //     node = next_cmd(shell, node);
    //     if (!node) // è NULL se è stato navigato tutto il left branch
    //         break;
    // }
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 18:01:09 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/21 11:43:25 by rdolzi           ###   ########.fr       */
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
    else if (!ft_strncmp(node->content.cmd[0], "exit", len, 1)) // ?
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

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// 2. executor & subshells

//    B.  PER EXECUTOR lvl_subshell: se termina senza operatore fuori dalle tonde,
//        effettivamente non esiste un nodo_op con lvl 0. quindi si puo
//        considerare il raggiungimento dell ultimo nodo come se ci fosse lo zero
//        esempio creazione is_last_cmd() se necessario


// -------------------- fd.c / execve.c / open_file.c --------------------
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
        // perror("Dup2 error");
        write(2, "Dup2 error", 10);
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
    printf("lvl:%d\n", lvl);
    printf("in ft_fd_sub_level...\n");
    while (++i < node->content.kv_size)
    {
       if (node->content.redir[i].lvl == lvl && (is_first == 0  && (node->content.redir[i].key == R_INPUT || node->content.redir[i].key == R_INPUT_HERE_DOC)))
       {
            printf("ft_fd_sub_level: FAft_open_file!\n");
            if(!ft_open_file(node, i))
                return (0);
        }
        else
        {
            if (!ft_open_file(node, i))
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
    // printf("in ft_fd_cmd_level...\n");
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
    
    if (node->content.cmd && node->content.cmd[0][0] != '/')
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
        write(2, "Error: command not found\n", 25);
        exit(30);
    }
}
// esegue in ordine:
// 1. ft_fd_sub_level
// 2. ft_fd_cmd_level
// se ok setta tali valori in std_...
// se ko fa il reset dei valori originali std_..
int ft_do_redir(t_node *node)
{
    int flag;
    int max_lvl;

    flag = 0;
    // printf("in ft_single_cmd...\n");
    if (node->content.redir)
        max_lvl = node->content.redir[node->content.kv_size - 1].lvl;
    else
        max_lvl = 0;
    // printf("max_lvl:%d\n", max_lvl);
    // esegue tutti gli fd cmd_level in ordine decrescente, non esegue lo zero
    while (max_lvl > 0)
    {
        // printf("max_lvl:%d\n", max_lvl);
        if (!ft_fd_sub_level(node, max_lvl--, flag))
        {
            // printf("esce in ft_fd_sub_level...\n");
            ft_reset_original_fd(node);
            return (0);
        }
        flag++;
    }
    // printf("PRE: ft_fd_cmd_level(node)\n");
    if (!ft_fd_cmd_level(node))
    {
        ft_reset_original_fd(node);
        return (0);
    }
    return (1);
}

// esegue il singolo cmd, ovvero:
// fa le redir (tranne here_doc che è gia fatta)
// if r_input fail, stampa errore
// altrimenti esegue il cmd
// reset fd_originali
void ft_single_cmd(t_node *node)
{
    int     status;
    pid_t   pid;
    
    if (!ft_do_redir(node))
    {
        node->shell->exit_status = 1;
        return;
    }
    if (is_builtin(node))
        execute_builtin(node, node->shell);
    else
    {
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

    ft_reset_original_fd(node);
    // printf("POST: ft_reset_original_fd|pid:%d\n",getpid());
}

void ft_do_subshell(t_node *node)
{
    (void) node;
    // 1. crea file fittizio in append (se non esiste già)
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

// dato un nodo, ritorna il prossimo nodo allo stesso lvl_subshell
// in sequenza che sia and o or.
// utilizza: next_cmd_same_lvl
t_node *go_next_cmd_and_or(t_node *node)
{
    t_node *temp;
    t_node *next_node;

    temp = node;
    next_node = NULL;
    // printf("-----------!CERCO: ..go_next_cmd_and_or...--------------\n\n");
    while (1)
    {
        next_node = next_cmd_same_lvl(node);
        temp = next_node;
        if (!next_node)
            break;
        if (next_node->content.op != PIPE)
        {
            // printf("trovato primo nodo stesso lvl.address:%p\n", next_node);
            return (next_node);
        }
    }
    return (NULL);
    }

// ritorna 1, se status ok ritorna 0 se status ko
// 1. check status
// prima di poter eseguire un nodo and o or 
// bisogna controllare exit_status precedente.
// uno stato di non errore è:
// se -1, perche è valore default
// se 0, perche un cmd ha fatto exit_success
// rischio perche non guardo se PIPE, ma il nodo dovrebbe essere
// stato correttamente filtrato in execute()
int ok_status(t_node *node)
{
    // printf("node->shell->exit_status:%d\n", node->shell->exit_status);
    if (node->shell->exit_status == -1 || node->shell->exit_status == 0)
    {
        // printf("ok_status:return (1)\n");
        return (1);
    }
    return (0);
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
                // printf("FINISCE QUI!\n");
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
                // printf("BB\n");
                execute_builtin(node, node->shell);
                exit(0);
            }
            else
                ft_execve(node);
            // printf("CC\n");
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
    // printf("HELLO!\n");
    return(next_cmd_same_lvl(node));
}


// ritorna il prossimo nodo, a parità di livello
// shell->lvl_subshell;
// si scorre tutto l albero fino a che non trova un nodo 
// tale che: node->lvl_subshell == shell->lvl_subshell;
// il nodo da ritornare è node->right. (verifica affermazione)
t_node *next_cmd_same_lvl(t_node *node)
{
    t_node *temp;
    t_node *next_node;

    temp = node;
    temp->done_lock = 1;
    next_node = NULL;
    // printf("-----------!CERCO: ..next_cmd_same_lvl...--------------\n\n");
    while (1)
    {
        next_node = next_cmd2(node->shell, temp);
        if (temp == next_node)
            return (NULL);
        else
            temp = next_node;
        // next_node->done_lock = 1;
        // -- print --
        // printf("printing next node ......\n");
        // printf("shell->lvl_subshell:%d|node->lvl_subshell:%d\n", next_node->shell->lvl_subshell, node->lvl_subshell);
        // if (!next_node)
        //     printf("next_node: (null)\n");
        // if (next_node)
        // {
        //     printf("next_node:%p\n", next_node);
        //     printf("check se questo nodo è corretto....\n");
        //     // print_node(node->shell, next_node);
        // }
        // -----------
        if (!next_node)
            break;
        if (next_node && next_node->shell->lvl_subshell == node->lvl_subshell)
        {
            // printf("trovato primo nodo stesso lvl.address:%p\n", next_node);
            return (next_node);
        }
    }
    return (NULL);
}

// V2: torna il generico next_cmd
// torna null se non trova nulla
// entra che è sempre un node_cmd, esce sempre node_cmd
t_node *next_cmd2(t_shell *shell, t_node *node)
{
    if (!is_left_branch(node) && node->back == shell->tree)
    {
        printf("return null!!\n");
        return NULL;
    }
    if (is_left_branch(node)) // siamo in un nodo sinistro, ma sempre check if node_op a destra
    {
        if (is_node_cmd(node->back->right))
            return (node->back->right);
        else
        {
            // si tratta di nodo_op, quindi vado a cercare il next_cmd
            return (go_to_starter_node(node->back->right));
        }
    }
    else
    { // siamo in un nodo destro

        if (is_node_cmd(node->back->back->right))
            return (node->back->back->right);
        else
            return (go_to_starter_node(node->back->back->right));
    }
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
    // esegue tutti gli here_doc settati in s_kv
    // ft_do_heredoc(shell); //TODO
    if (is_node_cmd(shell->tree))
    {
        printf("nodo op è assente...\n");
        ft_single_cmd(shell->tree);
        return ;
    }
    printf(" è presente almeno 1 nodo op...\n");
    node = go_to_starter_node(shell->tree->left);
    // V1: multi_cmd_without parantheses
    // tutti i nodi sullo stesso livello.
    while (1)
    {
        if (!node || node->done_lock == 1)
        {
            // printf("QUI\n");
            if (node && node->done_lock == 1)
            {
                // printf("QUa\n");
                // ft_reset_original_fd(node);
            }
            
            break;
        }
             
        if (node->back && node->back->content.op == PIPE)
        {
            printf("ft_do_pipe...\n");
            next_node = ft_do_pipe(node);
            printf("next_node:p%p\n", next_node);
        }
        else if (node->back && (node->back->content.op == AND || node->back->content.op == OR))
        {
            printf("ft_do_and_or...\n");
            next_node = ft_do_and_or(node);
            printf("next_node:p%p\n", next_node);
        }
        node = next_node;
    }
    // in generale per l ultimo cmd, se non op dopo? come esco da subshell ?
        // // questa funzione non esegue il cmd, ma prepara solo l environment.
        // if (node->back && node->back->lvl_subshell > 0)
        //     ft_do_subshell(node);    
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/02 17:51:56 by rdolzi            #+#    #+#             */
/*   Updated: 2023/07/13 01:23:38 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_node	*set_node_operator(t_shell *shell, t_node *back_node, int idx_start);
t_node	*set_node_cmd(t_shell *shell, t_node *back_node, int idx_start, int dir);

// 1. PRENDE LA RAW LINE
// 2. CONTROLLA SE ESISTE UN OPERATORE LOGICO, OVVERO VERIFICA CHE:
//     A. SIA PRESENTE LA SEGUENZA DI CHAR:  &&  ||
//     B. CHE TALE SEQUENZA NON SIA in_quotes()
// 3. SETTA IL NODO DI ORIGINE:
//     A. SE ESISTE, PRENDE L ULTIMO OPERATORE LOGICO A DESTRA COME NODO DI START.
//     B. SE NON ESISTE, ABBIAMO COME OPERATORI SOLO | e () E OPERATORI REDIREZIONE > etc..IN QUESTO CASO PRENDIAMO SEMPRE L ULTIMA PIPE DI DESTRA COME NODO DI ORIGINE.
// 4. SETTIAMO IL RAMO DI SINISTRA. TALE OPERAZIONE PREVEDE:
//     A. INDIVIDUAZIONE PROSSIMO OPERATORE LOGICO (&& || |)
//         AA. INDIVIDUARE LA PRESENZA DI TALI CARATTERI
//         BB. VERIFICARE CHE NON SIA in_quotes()
//     B. LA SEQUENZA DI CARATTERI DATA DALLA DIFFERENZA DELL INDEX DELL OPERATORE DI
//        START E QUELLO DI END DIVENTA IL RAW_CMD (che verrà elaborato dall executor)
// --------------------------------------------------------------------------------------

// INPUT:
// idx = indice di start, contiene l operatore
// dir = è la direzione (1 se verso destra, -1 se verso sinistra)
// COSA FA?
// restituisce la stringa con il comando raw
// ovvero tutti i char tra un operatore ed un altro(senza includerli)
// oppure se non trova un operatore nella direzione indicata, restituisce la stringa
// dal nodo di partenza fino a str[0] o str[len - 1], in base alla dir scelta.
// se va a sinistra il limiter sarà 0 altrimenti sarà len - 1
// questa stringa raw_cmd contiene ancora i comandi di redirection
// verrà ulteriormente filtrata dalla func set_redirection()
char	*get_raw_command(t_shell *shell, int idx, int dir)
{
	int		i;
	int		next_idx;
	char	*str;

	i = -1;
	next_idx = get_idx_next_op(shell, idx, dir);
	printf("in get_raw_command> idx:%d\n", idx);
	printf("in get_raw_command> next_idx:%d\n", next_idx);
	if (dir == 1)
	{
		if (next_idx == 0)
			next_idx = ft_strlen(shell->rawline) - 1; // ultimo index stringa
		printf("new next_idx:%d\n", next_idx);
		str = malloc(next_idx - idx + 1);
		if (idx) // se idx è operatore è da saltare se è 0 è da stampare
			idx++;
		next_idx++;
		while (idx < next_idx)
			str[++i] = shell->rawline[idx++];
	}
	else
	{
		str = malloc(idx - next_idx + 1);
		while (next_idx < idx)
			str[++i] = shell->rawline[next_idx++];
	}
	str[++i] = '\0';
	return (str);
}

// INPUT:
// 1.shell 2.index dell operatore
// ritorna l index del primo
// Setta il branch nella direzione indicata
// dir -1 è verso sinistra , dir 1 è verso destra
t_node	* set_branch(t_shell *shell, t_node *node, int idx_start, int dir)
{
	int	new_idx;

	new_idx = check_op_logic_than_pipe(shell, idx_start, dir);
	printf("in branch> next_op index: %d\n", new_idx);
	if (new_idx)
	{
		printf("in set_branch >set_node_operator..\n");
		return (set_node_operator(shell, node, new_idx));
	}
	else
	{
		printf("in set_branch >set_node_cmd..\n");
		return (set_node_cmd(shell, node, idx_start, dir));
	}
}

// V2 RICORSIVO
// PRESUPPOSTO CONCETTUALE:
// SE è NODO OPERATORE VADO A SETTARE I NODI BACK/LEFT/RIGHT E CONTENT SETTO
// SOLO LA STRINGA RAPPRESENTATIVA DELL OPERATORE get_op()
// ALTRIMENTI SETTO SOLO IL CONTENT CON IL PEZZO DI STRINGA DELIMITATO DA DUE NODI(4.B)
// O SE è L ULTIMO NODO, ALLORA LA STRINGA è DATA DAL NODO AL EOF o SOF(start of file)
// e setto I NODI BACK/LEFT/RIGHT a NULL
// lo stop alla ricorsività è dato dall index che si muove e arriverà eventualmente
// all EOF o SOF(start of file)
t_node	*set_node_operator(t_shell *shell, t_node *back_node, int idx_start)
{
	t_node *node;

	node = malloc(sizeof(*node));
	node->back = back_node;
	node->content.op = get_op(shell, idx_start);
	printf("idx_start: %d\n", idx_start);
	printf("operator: %d\n", node->content.op);
	printf("set_branch: LEFT..\n");
	node->left = set_branch(shell, node, idx_start, -1);
	printf("set_branch: RIGHT..\n");
	if (node->content.op == 10 || node->content.op == 11)
		node->right = set_branch(shell, node, idx_start + 1, 1);
	else
		node->right = set_branch(shell, node, idx_start, 1);
	return (node);
}

// INPUT:
// 1.Shell
// 2.back_node
// 3.idx_start
// 4.dir (3 & 4 vengono dati in input a get_raw_command)
// dir -1 è verso sinistra , dir 1 è verso destra
t_node	*set_node_cmd(t_shell *shell, t_node *back_node, int idx_start, int dir)
{
	t_node	*node;

	node = malloc(sizeof(*node));
	node->raw_cmd = get_raw_command(shell, idx_start, dir);
	if (node->raw_cmd)
	{
		printf("raw_cmd of get_raw_command:\n");
		printf("%s\n",node->raw_cmd); //seg fault a|b
	}
	node->back = back_node;
	node->left = NULL;
	node->right = NULL;
	node->content.op = 0;
	printf("set_content..\n");
	set_content(shell, node, 0);
	return (node);
}

// NELLA CREAZIONE DELL ALBERO VIENE PRIMA DETERMINATO
// SE è PRESENTE ALMENO 1 NODO OPERATORE, SI STARTERA' DA QUELLO.
// IN CASO CONTRARIO SIGNIFICA CHE ESISTE SOLAMENTE UN COMANDO "ATOMICO"
//  E BISOGNA SOLO CREARE UN NODO CONTENENTE IL SINGOLO COMANDO
// CHE COINCIDE CON LA rawline
void	set_tree(t_shell *shell)
{
	int	idx_start;

	idx_start = check_op_logic_than_pipe(
			shell, ft_strlen(shell->rawline) - 1, -1);
	printf("check_op_logic_than_pipe, index:%d\n", idx_start);
	if (idx_start)
	{
		printf("set_node_operator..\n");
		shell->tree = set_node_operator(shell, NULL, idx_start);
	}
	else
	{
		printf("set_node_cmd..\n");
		shell->tree = set_node_cmd(shell, NULL, 0, 1);
	}
}


/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:18:34 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/04 17:32:38 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// cd (Change Directory):

// Per implementare il comando cd, puoi utilizzare la funzione di libreria chdir()
//  per cambiare la directory di lavoro corrente. Assicurati di gestire i casi speciali,
//   come il cambio alla home directory (~) e il ritorno alla directory precedente (..).

void ft_cd(t_node *node)
{
    // char    *home;

    // home = getenv("HOME");
    printf("Start node cmd[0]:%s\n", node->content.cmd[0]);
     printf("Start node cmd[1]:%s\n", node->content.cmd[1]);

    if(node->content.cmd[1] == NULL)
        ft_home();
    else
    {
        if (chdir(node->content.cmd[1]) != 0) 
        {
            perror("Errore durante il cambio di directory");
                // Ritorna un valore negativo per indicare un errore
        }
            printf("Directory corrente cambiata in: %s\n", node->content.cmd[1]);
            // Aggiorna il percorso della directory corrente usata dalla tua Minishell
            // Ad esempio, potresti usare una variabile globale per memorizzarla
    }
}

void ft_home()
{
    char    *home;

    home = getenv("HOME");
    chdir(home);
    
    // if (home != NULL) {
    //     printf("Home directory: %s\\n", home);
    // } else {
    //     printf("Home directory not found.\n");
    // }
}
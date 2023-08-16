/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:17:51 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/16 05:17:13 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// pwd (Print Working Directory):
// Per implementare il comando pwd, puoi utilizzare la funzione di libreria getcwd()
// per ottenere il percorso della directory di lavoro corrente. Successivamente,
// stampa il percorso ottenuto sulla console utilizzando printf() o write().

void ft_pwd(void)
{
    char buff[4096];

    if (getcwd(buff, sizeof(buff)) != 0)
        printf("%s\n", buff);
    else
        perror("errore pwd");
}

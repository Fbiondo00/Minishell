/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:19:07 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/06 14:53:07 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// export:

// Per implementare il comando export, devi gestire la creazione e
// la modifica delle variabili d'ambiente nel tuo programma. Le variabili
//  d'ambiente sono generalmente memorizzate come coppie di chiavi e valori.
//  Puoi utilizzare le funzioni di libreria get() e setenv() per ottenere e impostare
//   le variabili d'ambiente, rispettivamente.
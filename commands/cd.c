/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:18:34 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/07/21 16:18:56 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// cd (Change Directory):

// Per implementare il comando cd, puoi utilizzare la funzione di libreria chdir()
//  per cambiare la directory di lavoro corrente. Assicurati di gestire i casi speciali,
//   come il cambio alla home directory (~) e il ritorno alla directory precedente (..).
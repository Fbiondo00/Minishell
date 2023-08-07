/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 16:19:07 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/07 17:23:14 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// export:

// Per implementare il comando export, devi gestire la creazione e
// la modifica delle variabili d'ambiente nel tuo programma. Le variabili
//  d'ambiente sono generalmente memorizzate come coppie di chiavi e valori.
//  Puoi utilizzare le funzioni di libreria get() e setenv() per ottenere e impostare
//   le variabili d'ambiente, rispettivamente.

// Funzione per scambiare due stringhe
void swap_(char **str1, char **str2) {
    char *temp = *str1;
    *str1 = *str2;
    *str2 = temp;
}

// Funzione Bubble Sort per ordinare l'array di stringhe
void bubble_sort_strings(char **strings, int num_strings)
 {
    for (int i = 0; i < num_strings - 1; i++)
     {
        for (int j = 0; j < num_strings - i - 1; j++)
         {
            if (ft_strcmp(strings[j], strings[j + 1]) > 0)
             {
                swap_(&strings[j], &strings[j + 1]);
            }
        }
    }
}

// int main() {
//     char *fruits[] = {"Banana", "Apple", "Orange", "Grapes", "Kiwi"};
//     int num_fruits = sizeof(fruits) / sizeof(fruits[0]);

//     printf("Array di stringhe non ordinato:\n");
//     for (int i = 0; i < num_fruits; i++) {
//         printf("%s\n", fruits[i]);
//     }

//     bubble_sort_strings(fruits, num_fruits);

//     printf("\nArray di stringhe ordinato:\n");
//     for (int i = 0; i < num_fruits; i++) {
//         printf("%s\n", fruits[i]);
//     }

//     return 0;
// }


void ft_export(t_shell *shell)
{
    int i;
    
    // while (shell->env[++i])
    //     printf("%s\n", shell->env[i]);
    // printf("\n\n\n\n\n\n\n\n");
    i = -1;
    while(shell->env[++i][0] && shell->env[i][0] != '_')
        bubble_sort_strings(shell->env, ft_strlen(*shell->env));
    i = -1;
    while (shell->env[++i])
        printf("declare -x %s\n", shell->env[i]);
}
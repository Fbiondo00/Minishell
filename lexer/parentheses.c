/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parentheses.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/26 16:00:36 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/26 19:36:14 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

//-------- NB  --------
// essendo ricorsiva lavoreremo sempre con ultima (
// modifica da op o ( precedente a sinistra e ) a destra
void remove_par(t_shell *shell, int idx)
{
    int i;
    int min;
    int max;
    char *chars;

    chars = "(|&";
    min = idx;
    while (--min >= 0)
    {
        if (ft_strchr(chars, shell->str[min]) && !in_quotes_str(shell, min))
            break;
    }
    max = idx;
    while (++max < ft_strlen(shell->str))
    {
        if (shell->str[max] == ')' && !in_quotes_str(shell, max))
            break;
    }
    i = -1;
    while (++i < ft_strlen(shell->str))
    {
        if (i >= min && i <= max)
            if (shell->str[i] == '(' || shell->str[i] == ')')
                shell->str[i] = 'a';
    }
}

// riceve idx di ( restituisce idx di )
int get_end_parentheses(t_shell *shell, int idx)
{
    while (++idx < ft_strlen(shell->str))
    {
        if (shell->str[idx] == ')' && !in_quotes_str(shell, idx))
            return (idx);
    }
    return (0);
}

// ritorna 1 se empty, 0 in caso contrario
// idx coincide con la parentesi di apertura
// 41 == )
int empty_parentheses(t_shell *shell, int idx)
{
    int len;

    len = ft_strlen(shell->str);
    while (++idx < len)
    {
        if (shell->str[idx] == 41 && !in_quotes_str(shell, idx))
            break;
        if (shell->str[idx] != ' ')
            return (0);
    }
    return (1);
}

// ritorna 1:
//  - se i caratteri (  ) sono dispari o se sono nell ordine sbagliato )(
// ritorna il numero di occorrenza "(":
//  - se i caratteri (  ) sono pari e nell ordine giusto
// 41 == )
// 40 == (
int unclosed_parentheses(t_shell *shell)
{
    int i;
    int left_p;
    int right_p;

    i = -1;
    left_p = 0;
    right_p = 0;
    while (++i < ft_strlen(shell->str))
    {
        if (shell->str[i] == 41 && !in_quotes_str(shell, i))
        {
            right_p++;
            if (right_p > left_p)
                return (-1);
        }
        else if (shell->str[i] == 40 && !in_quotes_str(shell, i))
            left_p++;
    }
    if (left_p != right_p)
        return (-1);
    return (left_p);
}

int check_parentheses(t_shell *shell)
{
    int left_p;

    left_p = unclosed_parentheses(shell);
    if (left_p == 0)
        return (0);
    if (left_p == -1)
        return (1);
    if (recursive(shell, 0, left_p, -1))
        return (1);
    return (0);
}

// 5 func
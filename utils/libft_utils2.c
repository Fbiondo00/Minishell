/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 15:46:42 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/10 15:58:24 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char *ft_strchr(char *s, char ch)
{
    int slen;
    int i;

    i = 0;
    slen = ft_strlen(s);
    while (i <= slen)
    {
        if (s[i] == ch)
            return ((char *)&s[i]);
        i++;
    }
    return (NULL);
}

// "00000"00000""0"000000"00000000''
// "ciao'"  ok "" "'come'"   stai ''
void print_str(char *str)
{
    int i;

    i = -1;
    while (++i < ft_strlen(str))
        printf("%c", str[i]);
    printf("\n");
}

int get_idx_eq_str(char *str)
{
    int i;

    i = -1;
    while (str[++i])
    {
        if (str[i] == '=')
            break ;
    }
    return (i); 
}

int ft_strncmp(char *s1, char *s2, int n, int flag)
{
    int i;
    unsigned char *us1;
    unsigned char *us2;

    i = 0;
    us1 = (unsigned char *)s1;
    us2 = (unsigned char *)s2;
     printf("str:%s|env[i]:%s\n\n", s1, s2);
    printf("ft_strlen(s1):%d| get_idx_eq_str(s2):%d\n", ft_strlen(s1), get_idx_eq_str(s2));
    if (flag && get_idx_eq_str(s1) !=  get_idx_eq_str(s2))
    {
        // printf("\n\n\nEsce in flag1\n");
        return (-999);
    }
    
    while ((us1[i] != '\0' || us2[i] != '\0') && i < n)
    {
        if (us1[i] == us2[i])
            i++;
        else
            
            return (us1[i] - us2[i]);
    }
    return (0);
}

char *ft_strdup(char *s1)
{
    char *s2;
    int len;
    int i;

    i = 0;
    len = ft_strlen(s1);
    s2 = (char *)malloc((len + 1) * sizeof(char));
    if (!s2)
        return (NULL);
    while (i < len)
    {
        s2[i] = s1[i];
        i++;
    }
    s2[i] = '\0';
    return (s2);
}

char *ft_substr(char *s, int start, int len)
{
    char *substr;
    int j;

    if (!s)
        return (NULL);
    j = 0;
    if (len > ft_strlen(s))
        len = ft_strlen(s);
    if (start >= ft_strlen(s))
        return (ft_strdup(""));
    substr = (char *)malloc((len + 1) * sizeof(char));
    if (!substr)
        return (NULL);
    while (j < len)
    {
        substr[j] = s[start];
        start++;
        j++;
    }
    substr[j] = '\0';
    return (substr);
}

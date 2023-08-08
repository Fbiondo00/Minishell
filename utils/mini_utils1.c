/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_utils1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 15:46:49 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/07 22:05:26 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void welcomeScreen()
{
    printf("\n-------------------------------------------------\n");
    printf("\tWelcome to 42 Minishell  \n");
    printf("\tPowered by : Rdolzi and Fbiondo (@42Roma)\n");
    printf("-------------------------------------------------\n");
    printf("\n\n");
}

char *ft_strjoin(char const *s1, char const *s2)
{
    size_t i;
    size_t j;
    char *s3;

    i = 0;
    j = 0;
    if (!s1)
        return (0);
    s3 = malloc(ft_strlen((char *)s1) + ft_strlen((char *)s2) + 1);
    if (!s3)
        return (0);
    while (s1[i])
        s3[j++] = s1[i++];
    i = 0;
    while (s2[i])
        s3[j++] = s2[i++];
    s3[j] = 0;
    return (s3);
}

char ft_strcmp(char *s1, char *s2)
{
    int i;

    i = 0;
    while (s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0')
        i++;
    return (s1[i] - s2[i]);
}

void ft_bzero(void *s, size_t n)
{
    size_t i;
    char *dest;

    i = 0;
    dest = (char *)s;
    while (i < n)
    {
        dest[i] = 0;
        i++;
    }
    return;
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
    size_t i;
    char *s;
    char *s2;

    if (!dest && !src)
        return (0);
    s = (char *)dest;
    s2 = (char *)src;
    i = 0;
    while (i < n)
    {
        s[i] = s2[i];
        i++;
    }
    return (dest);
}

void *ft_realloc(void *ptr, size_t size)
{
    void *new_ptr;

    if (ptr)
    {
        if (size)
        {
            new_ptr = malloc(size);
            if (!new_ptr)
                return (NULL);
            ft_bzero(new_ptr, size);
            ft_memcpy(new_ptr, ptr, size);
        }
        else
        {
            new_ptr = (unsigned char *)malloc(sizeof(ptr));
            if (!new_ptr)
                return (NULL);
        }
        free(ptr);
        return (new_ptr);
    }
    return ((unsigned char *)malloc(sizeof(ptr) * size));
}

int ft_get_len_mat(t_node *node)
{
    int ij;

    ij = 0;
    while(node->content.cmd[++ij])
        ;
    return (ij);
}
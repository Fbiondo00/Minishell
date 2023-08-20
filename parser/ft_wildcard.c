/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_wildcard.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/19 17:25:53 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/20 02:13:15 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


int ft_middle(char *str, int index)
{
    int i;
    int y;
    
    y = index;
    i = ft_strlen(str);

    while(++index < i)
        if(str[index] == '*')
            break ;
       while(--y > 0)
        if(str[y] == '*')
            break ;
        printf("\n asterick%c\n aterick%c\n", str[y], str[index]);
        if(str[y] == '*' && str[index] == '*')
                return (1);    
        return(0);
}

char *ft_asterisk(char *str)
{
    char *tem;
    char *new_str;
    int i;
	struct dirent *entry;
	const char *dirname = getcwd(0, 0);
    
    new_str = NULL;
	DIR *dir = opendir(dirname);
	if (dir == NULL)
	{
		perror("opendir() error");
		return ((char *)1);
	}
    while ((entry = readdir(dir)) != NULL)
    {
        i = -1;
        while (str[++i])
        {
            if (str[i] != '*')
            {   
                if(ft_middle(str,i))
                {
                    if(!ft_strchr(entry->d_name, str[i]))
                        break;
                     tem = ft_strjoin2(entry->d_name, " ");
                      printf("ft_sterrick tmp:%s\n", tem);
                    if (!new_str)
                        new_str = tem;
                    else
                        new_str = ft_strjoin2(new_str, tem);
                      printf("ft_sterrick str:%s\n", new_str);
                    free(tem);   
                    printf("ft_sterrick str mecchatielse:%s\n", new_str);
                    
                }
                else
                {
                     printf("\ncaraterri che non sono aterischi: %s\n", entry->d_name);
                    if (entry->d_name[i] != str[i])
                    break ;
                    tem = ft_strjoin2(entry->d_name, " ");
                      printf("ft_sterrick tmp:%s\n", tem);
                    if (!new_str)
                        new_str = tem;
                    else
                        new_str = ft_strjoin2(new_str, tem);
                      printf("ft_sterrick str:%s\n", new_str);
                    free(tem);   
                    printf("ft_sterrick str mecchatielse:%s\n", new_str);
                }
               
            }
        }
    }
  printf("ft_sterrick str mecchati:%s\n", new_str);
    free(str);
	closedir(dir);
    return (new_str);
}

void ft_wild(t_node *node, int i, int y)
{
    int max;
    int min;
    char *str;
    char *new_str;
    char *raw;
    char *quote;
    int ij;
    
    ij=0;
    max = i;
    while(node->raw_cmd[max])
    {
        if(node->raw_cmd[max] == 32 || (node->raw_cmd[max] == 39  || node->raw_cmd[max] == 34))
            break;
        max++;
    }
    printf("\nft_wildmax:%d\n", max);
    min = i;
 while(node->raw_cmd[min])
    {
        if(node->raw_cmd[min] == 32 || (node->raw_cmd[min] == 39  || node->raw_cmd[min] == 34))
            break;
        min--;
    }
    printf("\nft_wildmin: %d\n", min);
    str= malloc (max-min + 1);
         while(min <= max)
        str[ij++] = node->raw_cmd[++min];
    str[ij++] = 0;
    printf("ft_wildfinalestr:%s\n", str);
    new_str = ft_asterisk(str);
        printf("ft_wildfinalestr2:%s\n", new_str);
        raw = modify_raw_and_quote(node, y, new_str, 32);
        // free(node->raw_cmd);
        node->raw_cmd = raw;
       quote = modify_raw_and_quote(node, y, new_str, 48);
    //    free(node->quote_idx);
        node->quote_idx = quote;
}

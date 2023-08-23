/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_wildcard.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/23 01:45:48 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/23 13:43:51 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char *new_str_1(char *str)
{
    char *tem;
    char *new_str =NULL;
	struct dirent *entry;
    int flag;
      const char *dirname = getcwd(0, 0);
    DIR *dir = opendir(dirname);
    
    if (dir == NULL)
	{
		perror("opendir() error");
		return ((char *)1);
	}
     while ((entry = readdir(dir)) != NULL)
        {
            flag = flag1(entry->d_name,str);
            if (flag == 0) // significa che ha passato tutti i check richiesti e allora inserire in new_str
            {
                tem = ft_strjoin2(entry->d_name, " ");
                    printf("ft_sterrick tmp:%s\n", tem);
                if (!new_str)
                    new_str = ft_strjoin2(tem, " ");
                else
                new_str = ft_strjoin2(new_str, tem);
                free(tem);   
            }  
        }       
    free(str);
	closedir(dir);
    return (new_str);
}




int flag1(char *entry, char *str)
{
    int i;

    i = -1;
            while (str[++i]) // str = "s*po"  //d->name:echoppo[7- 4 +3 ] = [7-1]
            {
                if (str[i] != '*') // .."s"(fisso-left)..."i"(variab).."p"(fisso-right)
                {
                if(ft_middle(str,i))
                {
                    if(!ft_strchr(entry, str[i])) // ls *mm*
                        return(1);
                }
                else
                    {
                    if(check_left(str, i) == 1) // fisso-left
                    {
                            if (entry[i] != str[i])
                                return(1);
                    }
                    else if (check_left(str, i) == 0) // fisso-right
                            if (entry[ft_strlen(entry) - (ft_strlen(str) - i)] != str[i])
                                            return(1) ;                     
                    } 
                }
            }
        return(0);
}
//  flag = flag1(entry->d_name,str);


// torna 0 se asterisk è a destra
int check_left(char *str, int i)
{
    int y;

    y = 0;
    while (++i < ft_strlen(str))
    {
        if (str[i] == '*')
            return (1);
    }
    return (0);
    printf("return Y:%d\n", y);
}

int ft_stronly(const char *str, char c)
{
    while (*str)
    {
        printf("\nstr:control asterisk:%c\n", *str);
        if (*str != c)
            return 0; // Restituisce 0 se trova un carattere diverso da c
        str++;
    }
    return 1; // Restituisce 1 se tutti i caratteri sono c
}



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
        // printf("\n asterick%c aterick%c\n", str[y], str[index]);      
        if(str[y] == '*' && str[index] == '*')
                return (1);    
        return(0);
}

char  *ft_one()
{
    char *tem;
    char *new_str = NULL;
    struct dirent *entry;
	const char *dirname = getcwd(0, 0);
    DIR *dir = opendir(dirname);
    
    if (dir == NULL)
	{
		perror("opendir() error");
		return ((char *)1);
	}
     printf("entryft_one");
    while ((entry = readdir(dir)) != NULL)
    {
            tem = ft_strjoin2(entry->d_name, " ");
             if (!new_str)
                     new_str = ft_strjoin2(tem, " ");
            new_str = ft_strjoin2(new_str, tem);
            free(tem);
    }
    return (new_str);
}


int ft_max(t_node *node, int i)
{
    int max;
    
    max = i;
    while(node->raw_cmd[max])
    {
        if(node->raw_cmd[max] == 32 || (node->raw_cmd[max] == 39  || node->raw_cmd[max] == 34))
            break;
        max++;
    }
    max--;
    return(max);
}

int ft_min(t_node *node, int i)
{
    int min;
    
     min = i;
 while(node->raw_cmd[min])
    {
        if(node->raw_cmd[min] == 32 || (node->raw_cmd[min] == 39  || node->raw_cmd[min] == 34))
            break;
        min--;
    }
    min++;
    return(min);
}
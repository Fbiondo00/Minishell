/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_wildcard.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 23:21:04 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/08/22 00:48:13 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../minishell.h"

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
    char *new_str;
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
     printf("ft_wildfinalestr:%s\n", new_str);
            tem = ft_strjoin2(entry->d_name, " ");
             if (!new_str)
                     new_str = ft_strjoin2(tem, " ");
            new_str = ft_strjoin2(new_str, tem);
            free(tem);
    }
    return (new_str);
}


char *ft_asterisk(char *str)
{
    char *tem;
    char *new_str =NULL;
    int i;
	struct dirent *entry;
	const char *dirname = getcwd(0, 0);
    int flag;
    DIR *dir = opendir(dirname);
    
    flag = 0;
	if (dir == NULL)
	{
		perror("opendir() error");
		return ((char *)1);
	}
    if(ft_stronly(str, '*') == 1)
    {
        printf("entryft_one");
        new_str = ft_one();
        return(new_str);
    }
    else
    {
         while ((entry = readdir(dir)) != NULL)
        {
            i = -1;
            flag = 0;
            while (str[++i]) // str = "s*po"  //d->name:echoppo[7- 4 +3 ] = [7-1]
            {
                if (str[i] != '*') // .."s"(fisso-left)..."i"(variab).."p"(fisso-right)
                {
                    printf("str:%s|str[i:%d]:%c\n",str, i, str[i]);
                    // puo essere char fisso o variabile
                    // CHAR VARIABILE
                    if(ft_middle(str,i))
                    {
                        // printf("entra in fisso middle");
                         if(!ft_strchr(entry->d_name, str[i])) // ls *mm*
                            flag++;
                    }
                    else //CHAR FISSO
                    {
                         if(check_left(str, i) == 1) // fisso-left
                        {
                            // printf("entra in fisso left\n");
                            if (entry->d_name[i] != str[i])
                                flag++;
                        }
                        else if (check_left(str, i) == 0) // fisso-right
                        {
                            // printf("entra in fisso right\n%c %d", str[i], i);
                            if (entry->d_name[ft_strlen(entry->d_name) - (ft_strlen(str) - i)] != str[i])
                                {
                                                                    flag++ ;
                                }
                            // printf("\nentrydmane:%c &&&&  %s\n", entry->d_name[ft_strlen(entry->d_name) - (ft_strlen(str) - i)],entry->d_name );
                        }
                    } 
                }
            }
            if (flag == 0) // significa che ha passato tutti i check richiesti e allora inserire in new_str
            {
                tem = ft_strjoin2(entry->d_name, " ");
                    printf("ft_sterrick tmp:%s\n", tem);
                if (!new_str)
                    new_str = ft_strjoin2(tem, " ");
                    //   printf("ft_sterrick tmp:%s\n", tem);
                else
                new_str = ft_strjoin2(new_str, tem);
                printf("ft_sterrick str:%s\n", new_str);
                free(tem);   
            }  
        }       
    }
      printf("ft_sterrick str mecchati:%s\n", new_str);
    free(str);
	closedir(dir);
    return (new_str);
}



void ft_input(char *new_str, t_node *node, int y)
{
      char *raw;
    char *quote;
    
    if(new_str)
    {
        printf("ft_wildfinalestr2:%s\n", new_str);
        raw = modify_raw_and_quote2(node, y, new_str, 32);
        // free(node->raw_cmd);
        node->raw_cmd = raw ;
       quote = modify_raw_and_quote2(node, y, new_str, 48);
    //    free(node->quote_idx);
        node->quote_idx = quote;
    }
}
char *create_str(t_node *node, int max, int min)
{
     char *str;
     int ij;
    
    ij=0;
     str= malloc (max-min + 1);
    if(!str)
		return (NULL);
    while(min <= max &&(node->raw_cmd[min] != 32 && node->raw_cmd[min]))
        str[ij++] = node->raw_cmd[min++];
    str[ij++] = 0;
     printf("ft_wild_str:%s|str[i:%d]:\n",str, ft_strlen(str));
     return (str);
}

void ft_wild(t_node *node, int i, int y)
{
    int max;
    int min;
    char *str;
    char *new_str;

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
    min++;
    if(min<0)
        min = 0;
    printf("\nft_wildmin: %d\n", min);
    str = create_str(node, max, min);
    new_str = ft_asterisk(str);
    ft_input(new_str, node, y);
}



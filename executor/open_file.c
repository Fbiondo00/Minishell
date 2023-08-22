/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdolzi <rdolzi@student.42roma.it>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 21:53:08 by rdolzi            #+#    #+#             */
/*   Updated: 2023/08/22 23:58:24 by rdolzi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// esegue here_doc e poi chiude il file, verrà riaperto in ft_open
// inoltre sostituisce il nome della value redir, con il nome del nuovo file
// int ft_here_doc(int *fd, char *limiter)
// {
//     char *str;

//     str = NULL;
//     *fd = open("temp.txt", O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
//     if (*fd == -1)
//         return (0);
//     while ((ft_strncmp(str, limiter, ft_strlen(str) - 1, 0)) || (ft_strlen(str) - 1) != ft_strlen(limiter))
//     {
//         write(1, &">", 1);
//         str = get_next_line(0);
//         if ((ft_strncmp(str, limiter, ft_strlen(str) - 1, 0)) && write(*fd, str, ft_strlen(str)) == -1)
//             perror("Write error");
//         if (!str)
//         {
//             if (unlink("./temp.txt") != 0)
//                 perror("unlink error");
//         }
//         free(str);
//     }
//     close(*fd);
//     *fd = open("temp.txt", O_RDONLY, 0777);
//     return (1);
// }

int ft_here_doc(t_node *node, int i, char *new_value)
{
    int fd;
    char *str;
    char *path;

    printf("ENTRA IN FT_HERE_DOC\n");
    str = "123abc";
    fd = open(new_value, O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return (0);
    printf("limiter:%s\n", node->content.redir[i].value);
    while ((ft_strncmp(str, node->content.redir[i].value, ft_strlen(str) - 1, 0)) || (ft_strlen(str) - 1) != ft_strlen(node->content.redir[i].value))
    {
        write(1, &">", 1);
        str = get_next_line(0);
        if ((ft_strncmp(str, node->content.redir[i].value, ft_strlen(str) - 1, 0)) && write(fd, str, ft_strlen(str)) == -1)
            perror("Write error");
        if (!str)
        {
            path = ft_strjoin("./", new_value); // usare str, per ridurre righe?
            if (unlink(path) != 0)
                perror("unlink error");
            free(path);
        }
        free(str); // mettere in else ??
    }
    free(node->content.redir[i].value);
    node->content.redir[i].value = new_value;
    printf("create_signature:%s|len:%d\n", node->content.redir[i].value, ft_strlen(node->content.redir[i].value));
    close(fd);
    printf("ESCE DA FT_HERE_DOC\n");
    return (1);
}


int ft_open(int *fd, char *str, int key)
{
    if (key == R_OUTPUT_APPEND)
    {
        // printf("FT_OPEN:key == R_OUTPUT_APPEND\n");
        *fd = open(str, O_RDWR | O_CREAT | O_APPEND | O_CLOEXEC, 0777);
    }
    else if (key == R_OUTPUT_TRUNC)
    {
        // printf("FT_OPEN:key == R_OUTPUT_TRUNC\n");
        *fd = open(str, O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0777);
    }
    else if (key == R_INPUT || key == R_INPUT_HERE_DOC)
    {
        // printf("FT_OPEN:key == R_INPUT\n");
        *fd = open(str, O_RDONLY | O_CLOEXEC, 0777);
    }
    if (*fd == -1)
        return (0);
    printf("ft_open: FILE APERTO!\n");
    return (1);
}

// esegue le seguenti operazioni sul redir[idx]
// open file
// se ok -> fa il dup2, con l fd relativo (se non input o se non -1)
// se ok -> ritorna null
// ritorna 1 se ok, 0 se errore
//-9. chiudere here_doc dopo utilizzo ?? salvare variabile in struct?
int ft_open_file(t_node *node, int i)
{
    int fd;

    // if (node->content.redir[i].key == R_INPUT_HERE_DOC)
    //     ft_here_doc(&fd, node->content.redir[i].value);
    // else
        ft_open(&fd, node->content.redir[i].value, node->content.redir[i].key);
    if (fd == -1)
    {
        perror("Open error");
        return (0);
    }
    if (node->content.redir[i].fd != -1 && (node->content.redir[i].key == R_OUTPUT_TRUNC || node->content.redir[i].key == R_OUTPUT_APPEND))
    {
        // printf("node->content.redir[i].fd != -1 && KEY:R_OUTPUT_TRUNC|| R_OUTPUT_APPEND");
        // printf("ft_dup2(&fd:%d, node->content.redir[i].fd:%d)\n", fd, node->content.redir[i].fd);
        ft_dup2(&fd, node->content.redir[i].fd);
    }
    else if (node->content.redir[i].key == R_INPUT || node->content.redir[i].key == R_INPUT_HERE_DOC)
    {
        printf("KEY:R_INPUT|| R_INPUT_HERE_DOC\n");
        // printf("ft_dup2(&fd:%d, node->content.redir[i].fd:%d)\n", fd, STDIN_FILENO);
        ft_dup2(&fd, 0);
    }
    else if (node->content.redir[i].key == R_OUTPUT_TRUNC || node->content.redir[i].key == R_OUTPUT_APPEND)
    {
        // printf("KEY:R_OUTPUT_TRUNC|| R_OUTPUT_APPEND");
        // printf("ft_dup2(&fd:%d, node->content.redir[i].fd:%d)\n", fd, STDOUT_FILENO);
        ft_dup2(&fd, 1);
    }
    return (1);
}

// malloca nome del file da creare per here_doc
char *create_signature(t_node *node, int i, int k)
{
    char *str;
    char *alf;

    alf="ABCDEFGHILMNOPQRSTUVZ123456789";
    printf("vale of k: %d\n", k);
    str = ft_strjoin(node->content.redir[i].value, alf + ft_strlen(alf) - 1 - k);
    printf("create_signature:%s|len:%d\n", str, ft_strlen(str));
    return (str);
}

// si scorre tutto l albero ed esegue tutti gli here_doc settati, poi li richiude.
// verranno in seguito aperti da fd_do_redir. 
// cambio nome del value, cosi diventa poi un classico R_INPUT
// input: nodo di start albero.
void ft_do_heredoc(t_node *node)
{
    int i;
    static int k;
    t_node *temp;
    t_node *next_node;

    printf("IN ft_do_heredoc\n");
    k = 0;
    temp = node;
    next_node = NULL;
    while (1)
    {
        i = -1;
        while (++i < temp->content.kv_size)
        {
            
            // printf("A\n");
            if (temp->content.redir[i].key == R_INPUT_HERE_DOC)
            {
                ft_here_doc(temp, i, create_signature(temp, i, k++));
                printf("redir modificata [i:%d]..\n", i);
                print_node(temp->shell, temp);
            }
        }
        next_node = next_cmd2(temp->shell, temp);
        printf("next_node:%p\n", next_node);
        if (!next_node || temp == next_node )
            return ;
        else
            temp = next_node;
    }
}

// 5 func
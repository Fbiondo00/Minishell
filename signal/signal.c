/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flaviobiondo <flaviobiondo@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 18:01:04 by flaviobiond       #+#    #+#             */
/*   Updated: 2023/07/21 14:33:02 by flaviobiond      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
 
void	ft_head(int sign)
{
	t_shell shell;
	if (sign == SIGINT)
	{
		write(1, "\n", 1);
		//rl_replace_line("", 1);
		rl_on_new_line();
		rl_redisplay();
		shell.error = 130;
	}
}

void	ft_does_nothing(int signum)
{
	(void)signum;
	return ;
}

void	ft_handle_quit(int signum)
{
	if (signum != SIGQUIT)
		return ;
}

void	ft_signals(t_shell *shell)
{
	//salva i paramatetri nella struttura
	tcgetattr(STDIN_FILENO, &shell->tty_attrs);
	//disattivando ECHOCTL non visualizza i caratteri di controllo nel terminale.
	shell->tty_attrs.c_lflag &= ~(ECHOCTL);
	//applica i nuovi parametri
	tcsetattr(STDIN_FILENO, TCSANOW, &shell->tty_attrs);
	shell->signal_nothing.sa_handler = ft_does_nothing;
	shell->signal_int.sa_handler = ft_head;
	shell->signal_quit.sa_handler = ft_handle_quit;
	shell->signal_nothing.sa_flags = SA_RESTART;
	shell->signal_int.sa_flags = SA_RESTART;
	shell->signal_quit.sa_flags = SA_RESTART;
	sigemptyset(&(shell->signal_nothing.sa_mask));
	sigemptyset(&(shell->signal_int.sa_mask));
	sigemptyset(&(shell->signal_quit.sa_mask));
	sigaction(SIGINT, &shell->signal_int, 0);
	sigaction(SIGQUIT, &shell->signal_quit, 0);
}
void	ft_reset_signal(void)
{
	struct sigaction	signal;
	struct termios		attribute;

	tcgetattr(STDIN_FILENO, &attribute);
	attribute.c_lflag &= ~(ECHOCTL);
	tcsetattr(STDIN_FILENO, TCSANOW, &attribute);
	signal.sa_handler = NULL;
	signal.sa_flags = SA_RESTART;
	sigemptyset(&signal.sa_mask);
	sigaction(SIGINT, &signal, 0);
	sigaction(SIGQUIT, &signal, 0);
}
int	ft_continue(t_shell *shell, int n)
{
	if (n == 1 && (!shell->rawline || !shell->rawline[0]))
	{
		shell->error = 0;
		return (1);
	}
	else if (n == 2 && !shell->env)
	{
		shell->error = 0;
		return (2);
	}
	else if (n == 3 && (!shell->env || !shell->env[0]
			|| !shell->env[0][0]))
	{
		shell->error = 0;
		return (3);
	}
	return (0);
}
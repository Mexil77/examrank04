/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microsh.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgarcia <emgarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/24 13:22:19 by emgarcia          #+#    #+#             */
/*   Updated: 2022/01/25 15:58:46 by emgarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microsh.h"

void	ft_printall(t_line *line)
{
	t_line	*tmp;
	size_t	i;

	tmp = line;
	while (tmp)
	{
		i = -1;
		while (++i < tmp->size)
			printf("cmd[%zu] : %s\n", i, tmp->cmd[i]);
		tmp = tmp->next;
	}
}

size_t	ft_getsize(char **cmds)
{
	size_t	i;

	if (!cmds)
		return (0);
	i = 0;
	while (cmds[i] && strcmp(cmds[i], "|") != 0 && strcmp(cmds[i], ";") != 0)
		i++;
	return (i);
}

size_t	ft_strlen(char *str)
{
	size_t	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	ft_superexit(void)
{
	write(STDERR, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(EXIT_FAILURE);
}

char	*ft_strdup(char *str)
{
	size_t	i;
	char	*newstr;

	if (!str)
		return (NULL);
	newstr = malloc(sizeof(char) * (ft_strlen(str) + 1));
	if (!newstr)
		return (NULL);
	newstr[ft_strlen(str)] = '\0';
	i = -1;
	while (str[++i])
		newstr[i] = str[i];
	return (newstr);
}

size_t	ft_typeend(char	*cmd)
{
	if (!cmd)
		return (TYPE_END);
	if (!strcmp(cmd, "|"))
		return (TYPE_PIPE);
	if (!strcmp(cmd, ";"))
		return (TYPE_BREAK);
	return (0);
}

void	ft_listadd_back(t_line **line, t_line *new)
{
	t_line	*tmp;

	if (!(*line))
		*line = new;
	else
	{
		tmp = *line;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
		new->prev = tmp;
	}
}

size_t	ft_parsecmd(t_line **line, char **cmds)
{
	size_t	size;
	size_t	i;
	t_line	*new;

	size = ft_getsize(cmds);
	new = (t_line *)malloc(sizeof(t_line));
	if (!new)
		ft_superexit();
	new->cmd = (char **)malloc(sizeof(char *) * (size + 1));
	if (!new->cmd)
		ft_superexit();
	new->size = size;
	new->next = NULL;
	new->prev = NULL;
	new->cmd[size] = NULL;
	i = -1;
	while (++i < size)
		new->cmd[i] = ft_strdup(cmds[i]);
	new->type = ft_typeend(cmds[i]);
	ft_listadd_back(line, new);
	return (size);
}

size_t	ft_exitcd1(void)
{
	write(STDERR, "error: cd: bad arguments\n",
		ft_strlen("error: cd: bad arguments\n"));
	return (EXIT_FAILURE);
}

size_t	ft_exitcd2(char *str)
{
	write(STDERR, "error: cd: cannot change directory to ",
		ft_strlen("error: cd: cannot change directory to "));
	write(STDERR, str, ft_strlen(str));
	write(STDERR, "\n", 1);
	return (EXIT_FAILURE);
}

void	ft_exitexecve(char *cmd)
{
	write(STDERR, "error: cannot execute ",
		ft_strlen("error: cannot execute "));
	write(STDERR, cmd, ft_strlen(cmd));
	write(STDERR, "\n", 1);
	exit (EXIT_FAILURE);
}

void	ft_exectcmd(t_line *tmp, char **env)
{
	pid_t	pid;
	int		status;
	size_t	pipeopen;

	pipeopen = 0;
	if (tmp->type == TYPE_PIPE || (tmp->prev && tmp->prev->type == TYPE_PIPE))
	{
		pipeopen = 1;
		if (pipe(tmp->pipe))
			ft_superexit();
	}
	pid = fork();
	if (pid < 0)
		ft_superexit();
	else if (!pid)
	{
		if (tmp->type == TYPE_PIPE && dup2(tmp->pipe[STDOUT], STDOUT) < 0)
			ft_superexit();
		if (tmp->prev && tmp->prev->type == TYPE_PIPE
			&& dup2(tmp->prev->pipe[STDIN], STDIN) < 0)
			ft_superexit();
		if (execve(tmp->cmd[0], tmp->cmd, env) < 0)
			ft_exitexecve(tmp->cmd[0]);
		exit(EXIT_SUCCESS);
	}
	else
	{
		waitpid(pid, &status, 0);
		//write(STDERR, "Llegue\n", 7);
		if (pipeopen)
		{
			close(tmp->pipe[STDOUT]);
			if (!tmp->next || tmp->type == TYPE_BREAK)
				close(tmp->pipe[STDIN]);
		}
		if (tmp->prev && tmp->prev->type == TYPE_PIPE)
			close(tmp->prev->pipe[STDIN]);
	}
}

void	ft_exectline(t_line *line, char **env)
{
	t_line	*tmp;

	tmp = line;
	while (tmp)
	{
		if (!strcmp(tmp->cmd[0], "cd"))
		{
			if (tmp->size < 2)
				ft_exitcd1();
			else if (chdir(tmp->cmd[1]))
				ft_exitcd2(tmp->cmd[1]);
		}
		else
		{
			ft_exectcmd(tmp, env);
		}
		tmp = tmp->next;
	}
}

void	ft_freeall(t_line *line)
{
	t_line	*tmp;
	size_t	i;

	while (line)
	{
		tmp = line->next;
		i = -1;
		while (++i < line->size)
			free(line->cmd[i]);
		free(line->cmd);
		free(line);
		line = tmp;
	}
	line = NULL;
}

int	main(int argc, char *argv[], char **env)
{
	t_line	*line;
	size_t	i;

	if (argc > 1)
	{
		i = 0;
		while (argv[++i])
		{
			if (!strcmp(argv[i], ";"))
				continue ;
			i += ft_parsecmd(&line, &argv[i]);
			if (!argv[i])
				break ;
		}
		if (line)
			ft_exectline(line, env);
		ft_freeall(line);
	}
	return (0);
}

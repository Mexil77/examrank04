/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microsh.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgarcia <emgarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/24 13:22:41 by emgarcia          #+#    #+#             */
/*   Updated: 2022/01/25 13:20:06 by emgarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MICROSH_H
# define MICROSH_H

# define STDIN 0
# define STDOUT 1
# define STDERR 2

# define TYPE_END 3
# define TYPE_PIPE 4
# define TYPE_BREAK 5

# include <stdio.h> //delete
# include <stdlib.h>
# include <unistd.h>
# include <string.h>

typedef struct s_line
{		
	char			**cmd;
	size_t			size;
	size_t			type;
	int				pipe[2];
	struct s_line	*next;
	struct s_line	*prev;
}	t_line;

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maddi <maddi@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 00:28:45 by maddi             #+#    #+#             */
/*   Updated: 2022/03/31 05:46:38 by maddi            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int ft_handle_error(char *error)
{
	perror(error);
    return (0);
}

int ft_exec_cmd0(char **av, char ** envp, int *pip)
{
    int pid;
    t_cmd *cmd;
    
    cmd = malloc(sizeof(t_cmd));
    cmd->args = ft_split(av[2], ' ');
    cmd->binpath = ft_get_bin_path(envp, cmd->args);
    pid = fork();
    if (pid < 0)
        ft_handle_error("fork");
    if (!pid)
    {
        dup2(pip[WRITE], STDOUT_FILENO);
        close(pip[READ]);
        close(pip[WRITE]);
        execve(cmd->binpath, cmd->args, envp);
    }
    ft_delcmd(cmd);
}

int ft_exec_cmd1(char **av, char ** envp, int *pip)
{
    int pid;
    t_cmd *cmd;
    
    cmd = malloc(sizeof(t_cmd));
    cmd->args = ft_split(av[3], ' ');
    cmd->binpath = ft_get_bin_path(envp, cmd->args);
    pid = fork();
    if (pid < 0)
        ft_handle_error("fork");
    if (!pid)
    {
        dup2(pip[READ], STDIN_FILENO);
        close(pip[READ]);
        close(pip[WRITE]);
        execve(cmd->binpath, cmd->args, envp);
    }

    ft_delcmd(cmd);
}


int ft_open(char **av)
{
    int infile;
    int outfile;
    int dupret;
    
    if (!ft_strncmp(av[1], "/dev/stdin", 10))
        infile = STDIN_FILENO;
    else
        infile = open(av[1], O_RDONLY);
    if (infile < 0)
        return (ft_handle_error("infile"));
    outfile = open(av[4], O_CREAT| O_TRUNC| O_RDWR, 0777);
    if (outfile < 0)
        return (ft_handle_error("outfile"));
    dupret = dup2(infile, STDIN_FILENO);
    if (dupret < 0)
        return (ft_handle_error("dup2 infile"));     
    dupret = dup2(outfile, STDOUT_FILENO);
    if (dupret < 0)
        return (ft_handle_error("dup2 outfile"));
    close(outfile);
    close(infile);
    return (1);    
}

int main(int ac, char **av, char **envp)
{
    int openret;
    int pip[2];

    if (ac != 5)
        return (-1);
    pipe(pip);   
    openret = ft_open(av);
    if (openret == 0)
        return (1);
    ft_exec_cmd0(av, envp, pip);
    ft_exec_cmd1(av, envp, pip);
    waitpid(-1, NULL, -1);
    close(pip[READ]);
    close(pip[WRITE]);
}

#include "parser.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

/*
    REFACTOR SUGGESTIONS:
        - Refactor single command into method
        - refactor execute_pipeline_X methods, too redundant
*/

using namespace std;

void execute_pipeline(parsed_input *input);
void execute_pipeline_pline(pipeline *pline);
// void execute_subshell(parsed_input *input);

int main()
{
    string s_line;

    while (true)
    {
        // cout << std::flush;
        cout << "/> ";
        // Get line from cin
        getline(cin, s_line);
        char line[s_line.length() + 1];
        strcpy(line, s_line.c_str());
        // cout << "The line is: " << line << endl;

        // Check for quit command
        if (strcmp(line, "quit") == 0)
        {
            exit(0);
        }
        // Parse the input
        parsed_input *input = new parsed_input;
        parse_line(line, input);
        // TODO: For testing purposes, remove it after
        // pretty_print(input);

        // Case 1: Single command
        if (input->num_inputs == 1 && input->inputs[0].type == INPUT_TYPE_COMMAND)
        {
            pid_t pid = fork();
            if (pid == 0) // Child executes command
            {
                char **args = input->inputs[0].data.cmd.args;

                if ((execv(args[0], args)) == -1)
                {
                    execvp(args[0], args);
                }
            }
            else // Parent reaps the child
            {
                int status;
                pid_t wpid = waitpid(pid, &status, NULL);
            }
        }
        else if (input->separator == SEPARATOR_PIPE) // Case 2: Pipeline Execution
        {
            execute_pipeline(input);
        }
        else if (input->separator == SEPARATOR_SEQ) // Case 3: Sequential Execution
        {
            int n = input->num_inputs;
            for (int i = 0; i < n; i++)
            {
                if (input->inputs[i].type == INPUT_TYPE_PIPELINE) // If pipeline, execute pipeline
                {
                    execute_pipeline_pline(&input->inputs[i].data.pline);
                }
                else // If command execute command and reap it
                {
                    pid_t pid = fork();
                    if (pid == 0) // Child executes command
                    {
                        char **args = input->inputs[i].data.cmd.args;

                        if ((execv(args[0], args)) == -1)
                        {
                            execvp(args[0], args);
                        }
                    }

                    int status;
                    waitpid(pid, &status, NULL);
                }
            }
        }
        else if (input->separator == SEPARATOR_PARA) // Case 4: Parallel Execution
        {
            int n = input->num_inputs;
            for (int i = 0; i < n; i++)
            {
                if (input->inputs[i].type == INPUT_TYPE_PIPELINE) // If pipeline, fork so not waiting for pipeline execution. Child executes pipeline and exits.
                {
                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        execute_pipeline_pline(&input->inputs[i].data.pline);
                        exit(0);
                    }
                }
                else // Else, execute the command but not wait it.
                {
                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        char **args = input->inputs[i].data.cmd.args;

                        if ((execv(args[0], args)) == -1)
                        {
                            execvp(args[0], args);
                        }
                    }
                }
            }

            for (int i = 0; i < n; i++)
            {
                int status;
                wait(&status);
            }
        }
        else if (input->separator == SEPARATOR_NONE)
        { // Single subshell
            char *subshell_str = input->inputs[0].data.subshell;

            pid_t pid = fork();
            if (pid == 0)
            { // Child subshell process
                parsed_input *subshell_input = new parsed_input;
                parse_line(subshell_str, subshell_input);
                if (subshell_input->num_inputs == 1 && subshell_input->inputs[0].type == INPUT_TYPE_COMMAND) // SUBSHELL SINGLE COMMAND
                {
                    pid_t pid = fork();
                    if (pid == 0) // Child executes command
                    {
                        char **args = subshell_input->inputs[0].data.cmd.args;

                        if ((execv(args[0], args)) == -1)
                        {
                            execvp(args[0], args);
                        }
                    }
                    else // Parent reaps the child
                    {
                        int status;
                        pid_t wpid = waitpid(pid, &status, NULL);
                    }
                }
                else if (subshell_input->separator == SEPARATOR_PIPE) // SUBSHELL PIPELINE
                {
                    execute_pipeline(subshell_input);
                }
                else if (subshell_input->separator == SEPARATOR_SEQ) // SUBSHELL SEQUENTIAL
                {
                    int n = subshell_input->num_inputs;
                    for (int i = 0; i < n; i++)
                    {
                        if (subshell_input->inputs[i].type == INPUT_TYPE_PIPELINE) // If pipeline, execute pipeline
                        {
                            execute_pipeline_pline(&subshell_input->inputs[i].data.pline);
                        }
                        else // If command execute command and reap it
                        {
                            pid_t pid = fork();
                            if (pid == 0) // Child executes command
                            {
                                char **args = subshell_input->inputs[i].data.cmd.args;

                                if ((execv(args[0], args)) == -1)
                                {
                                    execvp(args[0], args);
                                }
                            }

                            int status;
                            waitpid(pid, &status, NULL);
                        }
                    }
                }
                else // SUBSHELL PARALLEL
                {
                    // Create pipes for each PC
                    int n_ss = subshell_input->num_inputs; // BE CAREFUL, SAME NAME AS OUTSIDE NUM INPUTS
                    int s_pipefds[n_ss][2];

                    for (int j = 0; j < n_ss; j++)
                    {
                        if (subshell_input->inputs[j].type == INPUT_TYPE_PIPELINE) // If pipeline, fork so not waiting for pipeline execution. Child executes pipeline and exits.
                        {
                            pipe(s_pipefds[j]);
                            pid_t pid = fork();
                            if (pid == 0)
                            {
                                dup2(s_pipefds[j][0], 0);
                                for (int k = 0; k <= j; k++)
                                {
                                    close(s_pipefds[k][1]);
                                    close(s_pipefds[k][0]);
                                }

                                execute_pipeline_pline(&subshell_input->inputs[j].data.pline);
                                exit(0);
                            }
                        }
                        else // Else, execute the command but not wait it.
                        {
                            pipe(s_pipefds[j]);
                            pid_t pid = fork();
                            if (pid == 0)
                            {
                                dup2(s_pipefds[j][0], 0);
                                for (int k = 0; k <= j; k++)
                                {
                                    close(s_pipefds[k][1]);
                                    close(s_pipefds[k][0]);
                                }

                                char **args = subshell_input->inputs[j].data.cmd.args;

                                if ((execv(args[0], args)) == -1)
                                {
                                    execvp(args[0], args);
                                }
                            }
                        }
                    }

                    pid_t f_pid = fork(); // Fork repeater
                    if (f_pid == 0)
                    {
                        // char c;
                        // while ((c = getchar()) != EOF)
                        // {
                        //     for (int j = 0; j < n_ss; j++)
                        //     {
                        //         write(s_pipefds[j][1], &c, 1);
                        //     }
                        // }
                        // std::string read_input(std::istreambuf_iterator<char>(cin), {});
                        // cerr << read_input;
                        // string read_line, read_input;

                        // while (getline(cin, read_line))
                        // {
                        //     read_input.append(read_line + "\n");
                        // }
                        // cerr << "--START--" << endl;
                        // cerr << read_input.c_str() << endl;
                        // cerr << "--END--" << endl;

                        size_t read_bytes;
                        char buffer[256];
                        string read_input;

                        while ((read_bytes = read(STDIN_FILENO, buffer, 256)) > 0)
                        {
                            read_input.append(buffer, read_bytes);
                        }

                        for (int j = 0; j < n_ss; j++)
                        {
                            write(s_pipefds[j][1], read_input.c_str(), read_input.size());
                        }

                        for (int j = 0; j < n_ss; j++)
                        {
                            close(s_pipefds[j][1]);
                            close(s_pipefds[j][0]);
                        }

                        exit(0);
                    }

                    for (int j = 0; j < n_ss; j++)
                    {
                        close(s_pipefds[j][1]);
                        close(s_pipefds[j][0]);
                    }

                    for (int j = 0; j < n_ss + 1; j++)
                    {
                        int status;
                        wait(&status);
                    }
                }

                free_parsed_input(subshell_input);
                exit(0);
            }

            int status;
            waitpid(pid, &status, NULL);
        }

        // Free the parsed_input
        free_parsed_input(input);
    }
}

void execute_pipeline(parsed_input *input)
{
    int i; // CS (command or subshell) index
    int n = input->num_inputs;
    int pipe_fds[n - 1][2]; // create n-1 pipes
    for (i = 0; i < n; i++)
    {
        if (i < n - 1)
        {
            int pval = pipe(pipe_fds[i]);
            // cout << "Pipe: " << i << " created." << endl;
        }

        pid_t pid = fork();
        if (pid == 0) // set up pipe for child
        {

            if (i > 0)
            {
                // cout << "Command" << i << "set to read from pipe " << i - 1 << endl;
                dup2(pipe_fds[i - 1][0], 0); // child reads from the previous command
                close(pipe_fds[i - 1][1]);
                close(pipe_fds[i - 1][0]);
            }

            if (i < n - 1)
            {
                // cout << "Command " << i << "set to write pipe " << i << endl;
                dup2(pipe_fds[i][1], 1); // child writes to current pipe
                close(pipe_fds[i][0]);
                close(pipe_fds[i][1]);
            }

            if (input->inputs[i].type == INPUT_TYPE_COMMAND)
            {
                char **args = input->inputs[i].data.cmd.args;
                if ((execv(args[0], args)) == -1)
                {
                    execvp(args[0], args);
                }
            }
            else
            {
                char *subshell_str = input->inputs[i].data.subshell;
                parsed_input *subshell_input = new parsed_input;
                parse_line(subshell_str, subshell_input);
                if (subshell_input->num_inputs == 1 && subshell_input->inputs[0].type == INPUT_TYPE_COMMAND) // SUBSHELL SINGLE COMMAND
                {
                    pid_t pid = fork();
                    if (pid == 0) // Child executes command
                    {
                        char **args = subshell_input->inputs[0].data.cmd.args;

                        if ((execv(args[0], args)) == -1)
                        {
                            execvp(args[0], args);
                        }
                    }
                    else // Parent reaps the child
                    {
                        int status;
                        pid_t wpid = waitpid(pid, &status, NULL);
                    }
                }
                else if (subshell_input->separator == SEPARATOR_PIPE) // SUBSHELL PIPELINE
                {
                    execute_pipeline(subshell_input);
                }
                else if (subshell_input->separator == SEPARATOR_SEQ) // SUBSHELL SEQUENTIAL
                {
                    int n_ss = subshell_input->num_inputs;
                    for (int j = 0; j < n_ss; j++)
                    {
                        if (subshell_input->inputs[j].type == INPUT_TYPE_PIPELINE) // If pipeline, execute pipeline
                        {
                            execute_pipeline_pline(&subshell_input->inputs[j].data.pline);
                        }
                        else // If command execute command and reap it
                        {
                            pid_t pid = fork();
                            if (pid == 0) // Child executes command
                            {
                                char **args = subshell_input->inputs[j].data.cmd.args;

                                if ((execv(args[0], args)) == -1)
                                {
                                    execvp(args[0], args);
                                }
                            }

                            int status;
                            waitpid(pid, &status, NULL);
                        }
                    }
                }
                else // SUBSHELL PARALLEL
                {
                    // Create pipes for each PC
                    int n_ss = subshell_input->num_inputs; // BE CAREFUL, SAME NAME AS OUTSIDE NUM INPUTS
                    int s_pipefds[n_ss][2];

                    for (int j = 0; j < n_ss; j++)
                    {
                        if (subshell_input->inputs[j].type == INPUT_TYPE_PIPELINE) // If pipeline, fork so not waiting for pipeline execution. Child executes pipeline and exits.
                        {
                            pipe(s_pipefds[j]);
                            pid_t pid = fork();
                            if (pid == 0)
                            {
                                dup2(s_pipefds[j][0], 0);
                                for (int k = 0; k <= j; k++)
                                {
                                    close(s_pipefds[k][1]);
                                    close(s_pipefds[k][0]);
                                }

                                execute_pipeline_pline(&subshell_input->inputs[j].data.pline);
                                exit(0);
                            }
                        }
                        else // Else, execute the command but not wait it.
                        {
                            pipe(s_pipefds[j]);
                            pid_t pid = fork();
                            if (pid == 0)
                            {
                                dup2(s_pipefds[j][0], 0);
                                for (int k = 0; k <= j; k++)
                                {
                                    close(s_pipefds[k][1]);
                                    close(s_pipefds[k][0]);
                                }

                                char **args = subshell_input->inputs[j].data.cmd.args;

                                if ((execv(args[0], args)) == -1)
                                {
                                    execvp(args[0], args);
                                }
                            }
                        }
                    }

                    pid_t f_pid = fork(); // Fork repeater
                    if (f_pid == 0)
                    {
                        // char c;
                        // while ((c = getchar()) != EOF)
                        // {
                        //     for (int j = 0; j < n_ss; j++)
                        //     {
                        //         write(s_pipefds[j][1], &c, 1);
                        //     }
                        // }
                        // std::string read_input(std::istreambuf_iterator<char>(cin), {});
                        // cerr << read_input;
                        // string read_line, read_input;

                        // while (getline(cin, read_line))
                        // {
                        //     read_input.append(read_line + "\n");
                        // }
                        // cerr << "--START--" << endl;
                        // cerr << read_input.c_str() << endl;
                        // cerr << "--END--" << endl;

                        size_t read_bytes;
                        char buffer[256];
                        string read_input;

                        while ((read_bytes = read(STDIN_FILENO, buffer, 256)) > 0)
                        {
                            read_input.append(buffer, read_bytes);
                        }

                        for (int j = 0; j < n_ss; j++)
                        {
                            write(s_pipefds[j][1], read_input.c_str(), read_input.size());
                        }

                        for (int j = 0; j < n_ss; j++)
                        {
                            close(s_pipefds[j][1]);
                            close(s_pipefds[j][0]);
                        }

                        exit(0);
                    }

                    for (int j = 0; j < n_ss; j++)
                    {
                        close(s_pipefds[j][1]);
                        close(s_pipefds[j][0]);
                    }

                    for (int j = 0; j < n_ss + 1; j++)
                    {
                        int status;
                        wait(&status);
                    }
                }

                free_parsed_input(subshell_input);
                exit(0);
            }
        }

        if (i < n - 1)
        {
            close(pipe_fds[i][1]);
        }
    }

    // Close the read ends of pipes
    for (i = 0; i < n - 1; i++)
    {
        // cout << "Parent closing pipe: " << i << endl;
        // close(pipe_fds[i][0]);

        close(pipe_fds[i][1]);
        close(pipe_fds[i][0]);
    }

    // Reap the childs
    for (i = 0; i < n; i++)
    {
        // cout << "Waiting child: " << i << endl;
        int status;
        wait(&status);
    }
}

void execute_pipeline_pline(pipeline *input)
{
    int i; // CS (command or subshell) index
    int n = input->num_commands;
    int pipe_fds[n - 1][2]; // create n-1 pipes
    for (i = 0; i < n; i++)
    {
        if (i < n - 1)
        {
            int pval = pipe(pipe_fds[i]);
            // cout << "Pipe: " << i << " created." << endl;
        }

        pid_t pid = fork();
        if (pid == 0) // set up pipe for child
        {

            if (i > 0)
            {
                // cout << "Command" << i << "set to read from pipe " << i - 1 << endl;
                dup2(pipe_fds[i - 1][0], 0); // child reads from the previous command
                close(pipe_fds[i - 1][1]);
                close(pipe_fds[i - 1][0]);
            }

            if (i < n - 1)
            {
                // cout << "Command " << i << "set to write pipe " << i << endl;
                dup2(pipe_fds[i][1], 1); // child writes to current pipe
                close(pipe_fds[i][0]);
                close(pipe_fds[i][1]);
            }

            char **args = input->commands[i].args;
            if ((execv(args[0], args)) == -1)
            {
                execvp(args[0], args);
            }
        }

        if (i < n - 1)
        {
            close(pipe_fds[i][1]);
        }
    }

    // Close the read ends of pipes
    for (i = 0; i < n - 1; i++)
    {
        // cout << "Parent closing pipe: " << i << endl;
        // close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
        close(pipe_fds[i][0]);
    }

    // Reap the childs
    for (i = 0; i < n; i++)
    {
        // cout << "Waiting child: " << i << endl;
        int status;
        wait(&status);
    }
}

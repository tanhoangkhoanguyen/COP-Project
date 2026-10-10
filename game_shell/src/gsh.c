/*
Name: Tan Hoang Khoa Nguyen
NetID: tanhoangkhoa
Date: 10/02/2026
Description: Design a game shell that lets users 
- List and run games from a repository
- Change the repository path
- Redirect game input from a file
- Exit the shell
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

char error_message[30] = "An error has occurred\n";

// Every error has to print the same message
void error() {
    write(STDERR_FILENO, error_message, strlen(error_message));
    fflush(stderr);
}

// Check if a path exists and is a directory
int is_dir(char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

// Combine the repository path and game name
char *full_path(char *dir, char *name) {
    char *path = malloc(strlen(dir) + strlen(name) + 2);

    strcpy(path, dir);
    if (dir[strlen(dir) - 1] != '/')
        strcat(path, "/");

    strcat(path, name);
    return path;
}

// qsort to put game names in lexicographical order
int compare(const void *a, const void *b) { return strcmp(*(char **)a, *(char **)b); }

// List every game and get its description by running game --help
void ls_command(char *repo) {
    DIR *dir = opendir(repo);

    if (!dir) {
        error();
        return;
    }

    char *names[256];
    int count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") ||
            !strcmp(entry->d_name, ".."))
            // Directories, not games
            continue;

        names[count++] = strdup(entry->d_name);
    }

    closedir(dir);
    qsort(names, count, sizeof(char *), compare);

    for (int i = 0; i < count; i ++) {
        char *path = full_path(repo, names[i]);
        int pipefd[2];

        // The pipe lets the parent read what game --help prints
        if (pipe(pipefd) < 0) {
            printf("%s: (empty)\n", names[i]);
            free(path);
            free(names[i]);
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            close(pipefd[0]);
            close(pipefd[1]);

            printf("%s: (empty)\n", names[i]);

            free(path);
            free(names[i]);
            continue;
        }

        if (pid == 0) {
            close(pipefd[0]);

            // Make stdout go into the pipe instead of the terminal
            if (dup2(pipefd[1], STDOUT_FILENO) < 0)
                _exit(1);

            close(pipefd[1]);

            // argv[0] must be the game name, not its full path
            char *args[] = {names[i], "--help", NULL};

            execvp(path, args);
            _exit(1);
        }

        // Parent only reads from the pipe
        close(pipefd[1]);

        char output[4096];
        int total = 0, n, status;

        // Save everything printed by game --help
        while ((n = read(pipefd[0], output + total, sizeof(output) - total - 1)) > 0) {
            total += n;
            if (total == sizeof(output) - 1)
                break;
        }

        close(pipefd[0]);
        waitpid(pid, &status, 0);

        output[total] = '\0';
        printf("%s: ", names[i]);

        // Failed execution or no output means the description is empty
        if (total == 0 || !WIFEXITED(status) || WEXITSTATUS(status) != 0)
            printf("(empty)\n");
        else {
            printf("%s", output);
            if (output[total - 1] != '\n')
                printf("\n");
        }

        free(path);
        free(names[i]);
    }
}

// Run a game from the current repository
void run_game(char *repo, char **args, char *input) {
    char *path = full_path(repo, args[0]);
    int fd = -1;

    if (input) {
        fd = open(input, O_RDONLY);

        if (fd < 0) {
            // Input file cannot be opened
            error();
            free(path);
            return;
        }
    }

    pid_t pid = fork();
    if (pid < 0) {
        error();

        if (fd >= 0)
            close(fd);

        free(path);
        return;
    }

    if (pid == 0) {
        if (fd >= 0) {
            // Replace keyboard input with input from the given file
            if (dup2(fd, STDIN_FILENO) < 0) {
                error();
                _exit(1);
            }
            close(fd);
        }

        execvp(path, args);

        // execvp only returns if the game does not exist or cannot run
        error();
        _exit(1);
    }

    if (fd >= 0)
        close(fd);

    // Wait for the game before showing the next gsh prompt
    waitpid(pid, NULL, 0);
    free(path);
}

int main(int argc, char *argv[]) {
    // gsh must receive exactly one valid repository directory
    if (argc != 2 || !is_dir(argv[1])) {
        error();
        return 1;
    }

    char *repo = strdup(argv[1]);
    char *line = NULL;
    size_t size = 0;

    while (1) {
        printf("gsh> ");
        fflush(stdout);

        if (getline(&line, &size, stdin) == -1)
            break;

        // getline keeps '\n', but it should not be part of a command
        line[strcspn(line, "\n")] = '\0';

        char *args[256];
        int n = 0;

        // Split both spaces and tabs, since the assignment allows both
        char *token = strtok(line, " \t");

        while (token && n < 255) {
            args[n++] = token;
            token = strtok(NULL, " \t");
        }

        // execvp needs the argument array to end with NULL
        args[n] = NULL;

        // Empty lines are allowed, so just show another prompt
        if (n == 0)
            continue;

        // exit cannot have any arguments
        if (!strcmp(args[0], "exit")) {
            if (n != 1) {
                error();
                continue;
            }

            free(repo);
            free(line);

            exit(0);
        }

        // path needs exactly one valid directory
        if (!strcmp(args[0], "path")) {
            if (n != 2 || !is_dir(args[1])) {
                error();
                continue;
            }

            // Only replace the old path after the new path is valid
            free(repo);
            repo = strdup(args[1]);

            continue;
        }

        // ls is a built-in command and does not take arguments
        if (!strcmp(args[0], "ls")) {
            if (n != 1) {
                error();
                continue;
            }

            ls_command(repo);
            continue;
        }

        int pos = -1, cnt = 0;
        // Find '<' and make sure there is only one
        for (int i = 0; i < n; i++)
            if (!strcmp(args[i], "<")) {
                pos = i;
                cnt++;
            }

        char *input = NULL;
        if (cnt > 0) {
            if (cnt != 1 || pos == 0 || pos != n - 2) {
                error();
                continue;
            }

            input = args[pos + 1];
            args[pos] = NULL;
        }

        // Any command that is not built-in is treated as a game
        run_game(repo, args, input);
    }

    free(repo);
    free(line);
    return 0;
}
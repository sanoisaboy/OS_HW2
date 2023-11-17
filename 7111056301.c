#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

int commandMode = 1;
char cwd[1024];
char rootDir[1024];
char parentDir[1024];
extern char **environ;

char *currentDir(char *dir)
{
    char *curDir;
    curDir = malloc(100 * sizeof(char)); // 動態分配100個字符的空間

    if (strcmp(rootDir, dir) == 0)
    {
        free(curDir); // 不需要動態分配空間，直接返回常量字符串
        return "~";
    }

    // printf("%s\n", rootDir);
    // printf("%s\n", dir);
    // printf("%s\n", strstr(dir, rootDir) + strlen(rootDir));

    strcpy(curDir, "~");
    strcat(curDir, strstr(dir, rootDir) + strlen(rootDir));

    return curDir;
}

void printPrompt()
{
    char *username = getenv("USER");
    if (username != NULL)
    {
    }
    else
    {
        printf("Unable to determine the current user.\n");
    }

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
    }
    else
    {
        printf("Unable to determine the computer name.\n");
    }

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
    }
    else
    {
        perror("getcwd");
    }

    printf("%s@%s:%s$", username, hostname, currentDir(cwd));
}

void changeDirectory(char *word)
{
    char type[20];
    if (word == NULL)
    {
        chdir(rootDir);
        return;
    }
    // printf("%s", word);
    if (strcmp(word, "..") == 0)
    {
        strcpy(parentDir, cwd);

        if (strcmp(rootDir, parentDir) == 0)
            return;
        chdir(dirname(parentDir));
        return;
    }

    strcat(cwd, word);
    if (chdir(cwd) == 0)
    {
    }
    else
    {
        printf("cd: no such file or directory:%s\n", word);
    }
    /* strcpy(type, command);
    if (strcmp(type, "..") == 0)
    {
        printf("123");
    }
    printf("%d", strcmp(type, ".."));
    printf("%s", type);
    */
}
void pwd()
{
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        printf("pwd error");
    }
}

void export(char *word)
{
    char *variable;
    char *path;

    if (word == NULL)
    {
        // 當 word 為 NULL 時，獲取所有環境變數
        char **env = environ;
        while (*env != NULL)
        {
            printf("%s\n", *env);
            env++;
        }
    }
    else
    {
        variable = strtok(word, "=");
        path = strtok(NULL, " ");

        if (setenv(variable, path, 1) == 0)
        {
        }
        else
        {
            printf("Set environment variable error\n");
        }
    }
}

void shiftword(char *word, int start)
{
    size_t len = strlen(word);
    for (int i = start; i < len; i++)
    {
        word[i] = word[i + 1];
    }
    word[len - 1] = '\0';
}

void date()
{
    time_t now;
    struct tm *local_time;
    char output[30];

    time(&now);

    local_time = localtime(&now);

    strftime(output, sizeof(output), "%a %b %d %H:%M:%S %Z %Y", local_time);

    printf("%s\n", output);
}
void echoFunction(char *word)
{
    // printf("%s\n", word);
    if (word == NULL)
        return;
    if (strcmp(word, "`date` ") == 0)
    {
        date();
        return;
    }

    size_t len = strlen(word);
    int i = 0;
    while (i < len)
    {
        if (word[i] == 34)
        {
            shiftword(word, i);
            i--;
        }
        else if (word[i] == 92 && word[i + 1] == 34)
        {
            word[i] = ' ';
            word[i + 1] = 34;
            shiftword(word, i);
        }
        i++;
    }

    printf("%s\n", word);
}

void output(char *path, char *exeName, char *fileName)
{
    int fd;
    if (!fileName)
        fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else
        fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, 1) == -1)
    {
        perror("dup2");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);

    execlp(path, exeName, NULL);

    perror("execlp");
    exit(EXIT_FAILURE);
}

void externalCommand(char *command)
{

    pid_t pid = fork();
    char *path = malloc(strlen(cwd) + 1 + strlen(command) + 1);
    strcpy(path, cwd);
    strcat(path, "/");
    strcat(path, command);

    if (pid == 0)
    {

        execlp(path, command, NULL);

        perror("execlp");
        exit(EXIT_FAILURE);
    }
    else
    {

        int status;
        waitpid(pid, &status, 0);
    }
    free(path);
    return;
}
void runInBackground(char *command)
{

    char *path = malloc(strlen(cwd) + 1 + strlen(command) + 1);
    strcpy(path, cwd);
    strcat(path, "/");
    strcat(path, command);
    if (fork() == 0)
    {

        execlp(path, command, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }
    else
    {
    }
    free(path);
    return;
}

void runOutput(char *command, char *fileName)
{

    char *path = malloc(strlen(cwd) + 1 + strlen(command) + 1);
    strcpy(path, cwd);
    strcat(path, "/");
    strcat(path, command);
    pid_t pid = fork();
    if (pid == 0)
    {

        output(path, command, fileName);
    }
    else
    {

        int status;
        waitpid(pid, &status, 0);
    }

    free(path);
    return;
}

int main()
{

    char command[100];

    getcwd(rootDir, sizeof(rootDir));
    while (commandMode == 1)
    {

        printPrompt();

        fgets(command, 100, stdin);

        size_t len = strlen(command);
        if (len > 0 && command[len - 1] == '\n')
        {
            command[len - 1] = '\0';
        }

        char *commandType = strtok(command, " ");

        if (strcmp(commandType, "pwd") == 0)
        {
            pwd();
        }
        else if (strcmp(commandType, "cd") == 0)
        {
            // printf("%s", strtok(NULL, " "));
            changeDirectory(strtok(NULL, " "));
        }
        else if (strcmp(commandType, "export") == 0)
        {
            export(strtok(NULL, " "));
        }
        else if (strcmp(commandType, "echo") == 0)
        {
            char word[100] = "";
            char *token;
            token = strtok(NULL, " ");
            while (token != NULL)
            {
                strcat(word, token);
                strcat(word, " ");
                token = strtok(NULL, " ");
            }
            word[strlen(word)] = '\0';
            echoFunction(word);
        }
        else if (strncmp(command, "./", 2) == 0)
        {

            char *token;
            token = strtok(NULL, " ");

            char *slashPosition = strchr(command, '/') + 1;

            if (!token)
                externalCommand(slashPosition);
            else if (strcmp(token, "&") == 0)

                runInBackground(slashPosition);

            else if (strcmp(token, ">") == 0)
            {

                runOutput(slashPosition, strtok(NULL, " "));
            }
        }
        else if (strcmp(commandType, "exit") == 0)
        {
            break;
        }
        else
        {
            printf("command not found:%s\n", command);
        }
    }

    return 0;
}

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

void sighandler(int signo)
{
    signal(SIGTSTP, sighandler);
    printf("Cannot execute Ctrl+Z\n");
}

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
        // printf("Current user: %s\n", username);
    }
    else
    {
        printf("Unable to determine the current user.\n");
    }

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
        // printf("Computer name (hostname): %s\n", hostname);
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
    char *env;
    char *varible;
    char *path;
    printf("%s", word);
    varible = strtok(word, "=");
    path = strtok(NULL, " ");

    if (setenv(varible, path, 1) == 0)
    {
        // printf("set success");
    }
    else
    {
        printf("set env error");
    }
    env = getenv(varible);
    if (env != NULL)
    {
        printf("%s=%s\n", varible, env);
    }
    else
    {
        printf("get path error");
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
void echo(char *word)
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

void output()
{
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Redirect standard output to the file descriptor
    if (dup2(fd, 1) == -1)
    {
        perror("dup2");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Close the file descriptor (not needed after redirection)
    close(fd);

    // Now, anything written to standard output will be directed to "output.txt"
    // execlp("ls", "ls", NULL);
    execlp("/Users/sanoisaboy/OS_HW2/add", "add", "6", "4", NULL);
    // If execlp fails
    perror("execlp");
    exit(EXIT_FAILURE);
}

void externalCommand(char *command)
{
    pid_t pid = fork();
    // printf("====parent process start=====\n");
    if (pid == 0)
    {
        // setpgid(0, 0);
        // signal(SIGTSTP, sighandler);
        //   setpgid(0, 0);
        // printf("i am child process\n");
        // execlp("/Users/sanoisaboy/OS_HW2/add", "add", "6", "4", NULL);
        // execlp("/Users/sanoisaboy/OS_HW2/loop", "loop", NULL);

        // test > function
        output();
        /*
        execlp("/bin/sleep", "sleep", "5", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
        */
    }
    else
    {
        // printf("Parent process: PID=%d\n", getpid());
        int status;
        waitpid(pid, &status, 0);
        //  printf("%s\n", command);
        //  printf("background");
    }
    // printf("===== parent process end=====\n");
    return;
}
void runInBackground()
{

    if (fork() == 0)
    {
        // Child process
        // printf("Child process: PID=%d, PPID=%d\n", getpid(), getppid());
        execlp("/bin/sleep", "sleep", "5", NULL);
        // execlp("/Users/sanoisaboy/OS_HW2/loop", "loop", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        // printf("Parent process: PID=%d\n", getpid());
    }

    // 更換到想要執行的程序
    // execlp("/Users/sanoisaboy/OS_HW2/add", "add", "6", "4", NULL);

    // perror("Exec failed");
    // exit(EXIT_FAILURE);
    return;
}

int main()
{
    // date();
    char command[100];

    getcwd(rootDir, sizeof(rootDir));
    while (commandMode == 1)
    {
        printPrompt();
        fgets(command, 100, stdin);
        // scanf("[^\n]s", command);
        //  printf("%s\n", &command);
        // printf("%s", command);
        //  changeDirectory(&command);
        size_t len = strlen(command);
        if (len > 0 && command[len - 1] == '\n')
        {
            command[len - 1] = '\0';
        }
        // printf("%ld\n", len);
        char *commandType = strtok(command, " ");
        // char *exeName = strtok(command, "./");
        printf("%s\n", commandType);
        // printf("%s\n", exeName);
        //     printf("%d\n", strcmp(command, "export"));
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
            printf("%s", strtok(NULL, " "));
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
            echo(word);
        }
        else if (strstr(command, "./"))
        {
            // printf("%s\n", strtok(NULL, " "));
            // externalCommand(strtok(NULL, " "));
            // printf("%s\n", strstr(command, " t"));
            if (!strtok(NULL, " "))
                externalCommand(strtok(NULL, " "));
            else
                runInBackground();
        }
        else if (strcmp(commandType, "exit") == 0)
        {
            break;
        }

        // printf("%s\n", command);
    }

    // printPrompt();

    return 0;
}

#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>

int main()
{
    char *prompt = "$ ";
    while (1)
    {
        char *line = readline(prompt);
        printf("stdin:%s\n", line);
    }
    return 0;
}
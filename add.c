#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int var1 = atoi(argv[1]);
    int var2 = atoi(argv[2]);
    printf("%d+%d = %d\n", var1, var2, var1 + var2);
    return 0;
}
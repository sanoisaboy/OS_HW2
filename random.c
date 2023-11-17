#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    // 使用時間作為種子，以確保每次執行時產生不同的隨機數
    srand(time(NULL));

    // 打印 10 個隨機數字
    for (int i = 0; i < 10; ++i)
    {
        printf("%d ", rand() % 10);
    }

    printf("\n");

    return 0;
}

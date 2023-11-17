#include <stdio.h>
#include <curses.h>
#include <ctype.h>

#define MAX_BUFFER_SIZE 100

int main()
{
    initscr();            // 初始化 curses
    raw();                // 關閉行緩衝
    keypad(stdscr, TRUE); // 啟用特殊鍵
    noecho();             // 關閉輸入回顯

    printw("Enter text (press 'Enter' to submit, arrow keys to navigate):\n");
    refresh();

    int ch;
    char buffer[MAX_BUFFER_SIZE] = "";
    int bufferIndex = 0;

    while ((ch = getch()) != '\n')
    {
        if (ch == KEY_UP)
        {
            printw("\rUp arrow key pressed    ");
        }
        else if (ch == KEY_DOWN)
        {
            printw("\rDown arrow key pressed  ");
        }
        else if (ch == KEY_RIGHT)
        {
            printw("\rRight arrow key pressed ");
        }
        else if (ch == KEY_LEFT)
        {
            printw("\rLeft arrow key pressed  ");
        }
        else if (isprint(ch))
        {
            if (bufferIndex < MAX_BUFFER_SIZE - 1)
            {
                buffer[bufferIndex++] = ch;
                buffer[bufferIndex] = '\0';
            }
            printw("\rInput:%s", buffer);
        }
        else if (ch == 127) // ASCII code for backspace (退格鍵)
        {
            if (bufferIndex > 0)
            {
                buffer[--bufferIndex] = '\0';
                printw("\rInput:%s ", buffer);
            }
        }

        refresh();
    }

    printw("\nYou entered: %s\n", buffer);

    endwin(); // 結束 curses

    return 0;
}

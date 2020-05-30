#include "terminal.h"
#include "snk_util.h"
#include <windows.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>

#define TERM_TERMINAL_BUF_SIZE 65536

struct terminal_data_t {
    HANDLE hOutBuf;
    HANDLE hStdin;
    HANDLE hStdout;
    CHAR_INFO *terminalBuf;
    size_t terminalBufSize;
};

int
terminal_init(terminal_data_t **td)
{
    terminal_data_t *td_out;
    DWORD mode = 0;
    HANDLE hStdin;
    HANDLE hStdout;
    CONSOLE_CURSOR_INFO cursorInfo = { .dwSize = 1, .bVisible = FALSE };

    td_out = malloc(sizeof(*td_out));
    if (td_out == NULL)
        return ENOMEM;

    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE)
        return EFAULT;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE)
        return EFAULT;

    if (!GetConsoleMode(hStdin, &mode))
        return EFAULT;

    if (!SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT)))
        return EFAULT;

    if (!SetConsoleCursorInfo(hStdout, &cursorInfo))
        return EFAULT;;

    td_out->terminalBuf = malloc(TERM_TERMINAL_BUF_SIZE * sizeof(*td_out->terminalBuf));
    if (td_out->terminalBuf == NULL)
        return ENOMEM;

    td_out->terminalBufSize = TERM_TERMINAL_BUF_SIZE;
    td_out->hStdin = hStdin;
    td_out->hStdout = hStdout;
    *td = td_out;

    return 0;
}

void
terminal_fini(terminal_data_t *td)
{
    free(td);
}

int
terminal_get_char(terminal_data_t *td, int *c)
{
    INPUT_RECORD records[1];
    DWORD size = 0;
    DWORD n;
    int rc = 1;

    if (!GetNumberOfConsoleInputEvents(td->hStdin, &size))
        return rc;

    if (size == 0)
        return rc;

    while (--size > 0)
    {
        if (!ReadConsoleInput(td->hStdin, records, SNK_ARRAY_LEN(records), &n))
            continue;

        if (n > 0 && records[0].EventType == KEY_EVENT && records[0].Event.KeyEvent.bKeyDown)
        {
            *c = records[0].Event.KeyEvent.uChar.AsciiChar;
            rc = 0;
        }
    }

    return rc;
}

static uint32_t
terminal_min(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}

void
terminal_strcpy(CHAR_INFO *terminalBuf, const char *str, unsigned int lengthNeeded, unsigned int *lengthLeft)
{
    uint32_t i;

    unsigned int length = terminal_min(lengthNeeded, *lengthLeft);

    for (i = 0; i < length; i++)
    {
        terminalBuf[i].Char.AsciiChar = str[i];
    }

    *lengthLeft -= length;
}

int
terminal_draw(terminal_data_t *td, const char *draw_data, uint32_t width, uint32_t height,
              unsigned int n_players, const snk_score *score)
{
    CONSOLE_SCREEN_BUFFER_INFO info;

    COORD bufferSize;
    COORD bufferCoord = {0};
    SMALL_RECT writeRegion = {0};
    const char *scoreStr = "Score: ";
    unsigned int spaceLeft;
    unsigned int i;

    for (i = 0; i < td->terminalBufSize; i++)
    {
        td->terminalBuf[i].Char.AsciiChar = ' ';
        td->terminalBuf[i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }

    if (!GetConsoleScreenBufferInfo(td->hStdout, &info))
        return EFAULT;

    if (info.dwSize.X * info.dwSize.Y > td->terminalBufSize)
    {
        const char new_height = 20;

        assert(td->terminalBufSize > new_height);

        info.dwSize.Y = new_height;
        info.dwSize.X = td->terminalBufSize / new_height;
    }

    bufferSize = info.dwSize;
    writeRegion.Right = bufferSize.X - 1;
    writeRegion.Bottom = bufferSize.Y - 1;

    spaceLeft = bufferSize.X;
    terminal_strcpy(td->terminalBuf, scoreStr, strlen(scoreStr), &spaceLeft);
    for (i = 0; i < n_players; i++)
    {
        char playerScore[4] = {0};

        _snprintf(playerScore, sizeof(playerScore) - 1, "%u", score->player[i]);
        terminal_strcpy(td->terminalBuf + strlen(scoreStr) + i * (sizeof(playerScore) - 1),
                        playerScore, strlen(playerScore), &spaceLeft);
    }

    for (i = 0; i < height && i + 1 < bufferSize.Y; i++)
    {
        spaceLeft = bufferSize.X;
        terminal_strcpy(&td->terminalBuf[(i + 1) * bufferSize.X], &draw_data[i * width], width, &spaceLeft);
    }

    if (!WriteConsoleOutput(td->hStdout, td->terminalBuf, bufferSize, bufferCoord, &writeRegion))
        return EFAULT;

    return 0;
}

void
terminal_msleep(terminal_data_t *td, unsigned int milliseconds)
{
    TERM_UNUSED(td);

    Sleep(milliseconds);
}
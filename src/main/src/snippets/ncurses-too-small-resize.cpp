#include <ncurses.h>
#include <cstdio>
#include <cstdlib>

void initNcurses()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();
}

// Sends an xterm-style window-ops escape sequence asking the terminal
// emulator to resize itself. Best-effort only — not all terminals honor
// this (most graphical emulators do; tmux/screen and some consoles don't).
static void requestTerminalResize(int rows, int cols)
{
    printf("\033[8;%d;%dt", rows, cols);
    fflush(stdout);
}

// Displays a "too small" message and asks the user whether to resize.
// Returns true if the user agreed.
static bool promptUserToResize(int haveRows, int haveCols, int needRows, int needCols)
{
    clear();
    mvprintw(0, 0, "Terminal is too small for Craps.");
    mvprintw(1, 0, "Have: %d rows x %d cols", haveRows, haveCols);
    mvprintw(2, 0, "Need: %d rows x %d cols", needRows, needCols);
    mvprintw(4, 0, "Resize automatically? [y/n]: ");
    refresh();

    int ch = getch();
    return (ch == 'y' || ch == 'Y');
}

// Ensures the terminal is at least minRows x minCols, prompting the user
// to auto-resize if it's not. Returns true if the terminal meets the
// minimum (possibly after resizing); false means the caller should exit.
bool ensureMinimumTerminalSize(int minRows, int minCols)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    if (rows >= minRows && cols >= minCols) {
        return true;
    }

    if (!promptUserToResize(rows, cols, minRows, minCols)) {
        return false;   // user declined
    }

    requestTerminalResize(minRows, minCols);

    // Give the terminal a moment to act on the request and deliver
    // SIGWINCH before we re-check.
    napms(200);
    
    if (is_term_resized(minRows, minCols)) {
        resize_term(minRows, minCols);
    }
    
    getmaxyx(stdscr, rows, cols);
    if (rows >= minRows && cols >= minCols) {
        return true;
    }

    // Terminal didn't honor the request.
    clear();
    mvprintw(0, 0, "Unable to resize terminal automatically.");
    mvprintw(1, 0, "Please resize to at least %d x %d and restart.", minRows, minCols);
    refresh();
    return false;
}


int main()
{
    constexpr int kMinRows = 41;
    constexpr int kMinCols = 99;

    initNcurses();

    if (!ensureMinimumTerminalSize(kMinRows, kMinCols)) {
        endwin();
        std::fprintf(stderr, "Terminal too small; exiting.\n");
        fflush(stdout);
        return EXIT_FAILURE;
    }

    // ... proceed with game setup ...

    endwin();
    return EXIT_SUCCESS;
}

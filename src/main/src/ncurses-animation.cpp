#include <chrono>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <thread>
#include <string>
#include <wchar.h>

struct Layout {
    // Overall dimensions
    static constexpr int total_rows = 38;
    static constexpr int total_cols = 100;

    // Left/right split
    static constexpr int left_w  = 70;
    static constexpr int right_w = total_cols - left_w;

    // Row positions of horizontal dividers (y-coords)
    static constexpr int row_history = 2;
    static constexpr int row_table   = 4;
    static constexpr int row_message = 23;
    static constexpr int row_player  = 28;
    static constexpr int row_command = 35;
};

struct Windows
{
    WINDOW *header, *history, *table, *message;
    WINDOW *animation, *house, *player, *command;
};

void draw_frame()
{
    using L = Layout;

    // Horizontal lines
    mvhline(0,                0, ACS_HLINE, L::total_cols);
    mvhline(L::row_history,   0, ACS_HLINE, L::left_w);
    mvhline(L::row_table,     0, ACS_HLINE, L::left_w);
    mvhline(L::row_message,   0, ACS_HLINE, L::left_w);
    mvhline(L::total_rows-1,  0, ACS_HLINE, L::total_cols);

    mvhline(L::row_message, L::left_w, ACS_HLINE, L::right_w);
    mvhline(L::row_player,  L::left_w, ACS_HLINE, L::right_w);
    mvhline(L::row_command, L::left_w, ACS_HLINE, L::right_w);

    // Vertical lines
    mvvline(0, 0,               ACS_VLINE, L::total_rows);
    mvvline(0, L::total_cols-1, ACS_VLINE, L::total_rows);
    mvvline(0, L::left_w,       ACS_VLINE, L::total_rows);

    // Outer corners
    mvaddch(0,               0,               ACS_ULCORNER);
    mvaddch(0,               L::total_cols-1, ACS_URCORNER);
    mvaddch(L::total_rows-1, 0,               ACS_LLCORNER);
    mvaddch(L::total_rows-1, L::total_cols-1, ACS_LRCORNER);

    // Junctions at left border
    mvaddch(L::row_history, 0, ACS_LTEE);
    mvaddch(L::row_table,   0, ACS_LTEE);
    mvaddch(L::row_message, 0, ACS_LTEE);

    // Junctions at column split
    mvaddch(0,              L::left_w, ACS_TTEE);
    mvaddch(L::row_history, L::left_w, ACS_RTEE);
    mvaddch(L::row_table,   L::left_w, ACS_RTEE);
    mvaddch(L::row_message, L::left_w, ACS_PLUS);
    mvaddch(L::row_player,  L::left_w, ACS_LTEE);
    mvaddch(L::row_command, L::left_w, ACS_LTEE);
    mvaddch(L::total_rows-1,L::left_w, ACS_BTEE);

    // Junctions at right border
    mvaddch(L::row_message, L::total_cols-1, ACS_RTEE);
    mvaddch(L::row_player,  L::total_cols-1, ACS_RTEE);
    mvaddch(L::row_command, L::total_cols-1, ACS_RTEE);

    // Labels
    mvprintw(0,              2, "[Header]");
    mvprintw(L::row_history, 2, "[History]");
    mvprintw(L::row_table,   2, "[Table]");
    mvprintw(L::row_message, 2, "[Message]");

    mvprintw(0,              L::left_w+2, "[Animation]");
    mvprintw(L::row_message, L::left_w+2, "[House]");
    mvprintw(L::row_player,  L::left_w+2, "[Player]");
    mvprintw(L::row_command, L::left_w+2, "[Command]");

    refresh();
}

enum class Column { Left, Right };

WINDOW* make_subwin(int top_divider,
                    int bottom_divider,
                    int inner_w,
                    Column col)
{
    int start_y = top_divider + 1;
    int height  = bottom_divider - top_divider - 1;
    int start_x = (col == Column::Left) ? 1 : Layout::left_w + 1;

    return newwin(height, inner_w, start_y, start_x);
}

Windows create_subwindows()
{
    using L = Layout;
    Windows w{};

    int left_inner_w  = L::left_w  - 2;
    int right_inner_w = L::right_w - 2;

    // Left column
    w.header  = make_subwin(0,              L::row_history,  left_inner_w, Column::Left);
    w.history = make_subwin(L::row_history, L::row_table,    left_inner_w, Column::Left);
    w.table   = make_subwin(L::row_table,   L::row_message,  left_inner_w, Column::Left);
    w.message = make_subwin(L::row_message, L::total_rows-1, left_inner_w, Column::Left);

    // Right column
    w.animation = make_subwin(0,              L::row_message,  right_inner_w, Column::Right);
    w.house     = make_subwin(L::row_message, L::row_player,   right_inner_w, Column::Right);
    w.player    = make_subwin(L::row_player,  L::row_command,  right_inner_w, Column::Right);
    w.command   = make_subwin(L::row_command, L::total_rows-1, right_inner_w, Column::Right);

    return w;
}

#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

struct Die {
    int value;       // current face
    int framesLeft;  // how many frames left to roll
};

// draw a single die at position (y, x), jiggle applied outside
void draw_die(WINDOW *win, int y, int x, int value) {
    // erase 5x5 box
    for (int row = 0; row < 5; row++) {
        mvwprintw(win, y + row, x, "     ");
    }

    // borders
    mvwaddch(win, y,   x,   ACS_ULCORNER);
    mvwaddch(win, y,   x+4, ACS_URCORNER);
    mvwaddch(win, y+4, x,   ACS_LLCORNER);
    mvwaddch(win, y+4, x+4, ACS_LRCORNER);
    for (int i=1; i<4; i++) {
        mvwaddch(win, y,   x+i, ACS_HLINE);
        mvwaddch(win, y+4, x+i, ACS_HLINE);
        mvwaddch(win, y+i, x,   ACS_VLINE);
        mvwaddch(win, y+i, x+4, ACS_VLINE);
    }

    // enable bold for pips
    wattron(win, A_BOLD);

    // pip positions relative to (y, x)
    auto pip = [&](int row, int col) {
        mvwaddch(win, y+row, x+col, '*');
    };

    switch (value) {
        case 1: pip(2,2); break;
        case 2: pip(1,1); pip(3,3); break;
        case 3: pip(1,1); pip(2,2); pip(3,3); break;
        case 4: pip(1,1); pip(1,3); pip(3,1); pip(3,3); break;
        case 5: pip(1,1); pip(1,3); pip(2,2); pip(3,1); pip(3,3); break;
        case 6: pip(1,1); pip(1,3); pip(2,1); pip(2,3); pip(3,1); pip(3,3); break;
    }

    wattroff(win, A_BOLD);
}

void roll_dice(WINDOW *win) {
    Die dice[2];
    dice[0].framesLeft = 10 + rand() % 10;  // 10–19 frames
    dice[1].framesLeft = 10 + rand() % 10;

    while (dice[0].framesLeft > 0 || dice[1].framesLeft > 0) {
        for (int d=0; d<2; d++) {
            if (dice[d].framesLeft > 0) {
                dice[d].value = 1 + rand() % 6;
                dice[d].framesLeft--;

                // jiggle: offset -1, 0, or +1 columns
                int jiggle = (rand() % 3) - 1;
                int baseX = d==0 ? 2 : 10; // spacing between dice
                int x = baseX + jiggle;
                int y = 2;
                draw_die(win, y, x, dice[d].value);
            } else {
                // already stopped, redraw stable (no jiggle)
                int baseX = d==0 ? 2 : 10;
                draw_die(win, 2, baseX, dice[d].value);
            }
        }

        wrefresh(win);
        usleep(100000); // 100ms per frame
    }
}










#if 0
// Draw a mini dice box
void draw_die_box(WINDOW* win, int y, int x, int size = 5)
{
    mvwaddch(win, y, x, ACS_ULCORNER);
    for (int i = 1; i < size-1; i++) mvwaddch(win, y, x+i, ACS_HLINE);
    mvwaddch(win, y, x+size-1, ACS_URCORNER);

    for (int j = 1; j < size-1; j++) {
        mvwaddch(win, y+j, x, ACS_VLINE);
        mvwaddch(win, y+j, x+size-1, ACS_VLINE);
    }

    mvwaddch(win, y+size-1, x, ACS_LLCORNER);
    for (int i = 1; i < size-1; i++) mvwaddch(win, y+size-1, x+i, ACS_HLINE);
    mvwaddch(win, y+size-1, x+size-1, ACS_LRCORNER);
}

// Draw pips using asterisk (always one cell wide)
void draw_die_face(WINDOW* win, int y, int x, int value)
{
    chtype pip = '*' | A_BOLD;

    // box 5 rows x 7 cols
    mvwaddch(win, y,     x,     ACS_ULCORNER);
    mvwhline(win, y,     x+1,   ACS_HLINE, 5);
    mvwaddch(win, y,     x+6,   ACS_URCORNER);

    for (int j = 1; j <= 3; j++) {
        mvwaddch(win, y+j, x,   ACS_VLINE);
        mvwaddch(win, y+j, x+6, ACS_VLINE);
        for (int i = 1; i <= 5; i++) // clear inside
            mvwaddch(win, y+j, x+i, ' ');
    }

    mvwaddch(win, y+4,   x,     ACS_LLCORNER);
    mvwhline(win, y+4,   x+1,   ACS_HLINE, 5);
    mvwaddch(win, y+4,   x+6,   ACS_LRCORNER);

    // pip positions (spread across cols 2 and 4 with col 3 centered)
    auto pip_at = [&](int row, int col) {
        mvwaddch(win, y+row, x+col, pip);
    };

    switch(value) {
        case 1: pip_at(2, 3); break;
        case 2: pip_at(1, 2); pip_at(3, 4); break;
        case 3: pip_at(1, 2); pip_at(2, 3); pip_at(3, 4); break;
        case 4: pip_at(1, 2); pip_at(1, 4);
                pip_at(3, 2); pip_at(3, 4); break;
        case 5: pip_at(1, 2); pip_at(1, 4); pip_at(2, 3);
                pip_at(3, 2); pip_at(3, 4); break;
        case 6: pip_at(1, 2); pip_at(1, 4);
                pip_at(2, 2); pip_at(2, 4);
                pip_at(3, 2); pip_at(3, 4); break;
    }
}

void animate_dice_roll(WINDOW* win)
{
    using namespace std::chrono_literals;

    int left_x  = 2;
    int right_x = 10;
    int y       = 1;

    for (int i = 0; i < 8; i++)
    {
        int roll1 = (std::rand() % 6) + 1;
        int roll2 = (std::rand() % 6) + 1;

        draw_die_box(win, y, left_x);
        draw_die_face(win, y, left_x, roll1);

        draw_die_box(win, y, right_x);
        draw_die_face(win, y, right_x, roll2);

        wrefresh(win);
        std::this_thread::sleep_for(100ms);
    }
}
#endif


int main()
{
    std::srand(std::time(nullptr));
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    draw_frame();
    Windows w = create_subwindows();

    // Test writing into each window
    mvwprintw(w.header,  0, 0, "Header text here");
    mvwprintw(w.history, 0, 0, "History entry");
    mvwprintw(w.table,   0, 0, "Table content...");
    mvwprintw(w.message, 0, 0, "Messages go here");

    mvwprintw(w.animation, 0, 0, "Animation area");
    mvwprintw(w.house,     0, 0, "House: $1000");
    mvwprintw(w.player,    0, 0, "Player: $500");
    mvwprintw(w.command,   0, 0, "Command>");

    // Refresh all subwindows
    wnoutrefresh(w.header);
    wnoutrefresh(w.history);
    wnoutrefresh(w.table);
    wnoutrefresh(w.message);
    wnoutrefresh(w.animation);
    wnoutrefresh(w.house);
    wnoutrefresh(w.player);
    wnoutrefresh(w.command);
    doupdate();
    
    while (true)
    {
        curs_set(1); // cursor back on
        int ch = getch();
        if (ch == 'r')
        {
            curs_set(0); // cursor off
            werase(w.animation);
            roll_dice(w.animation);
        }
        else
        {
            break;
        }
    }

    endwin();
    return 0;
}

/*

0  ┌─[Header]────────────────────────────────────────────────────────────┬─[Animation]────────────────┐
1  │Header text here                                                     │Animation area              │
2  ├─[History]───────────────────────────────────────────────────────────┤                            │
3  │History entry                                                        │                            │
4  ├─[Table]─────────────────────────────────────────────────────────────┤                            │
5  │Table content...                                                     │                            │
6  │                                                                     │                            │
7  │                                                                     │                            │
8  │                                                                     │                            │
9  │                                                                     │                            │
10 │                                                                     │                            │
11 │                                                                     │                            │
12 │                                                                     │                            │
13 │                                                                     │                            │
14 │                                                                     │                            │
15 │                                                                     │                            │
16 │                                                                     │                            │
17 │                                                                     │                            │
18 │                                                                     │                            │
19 │                                                                     │                            │
20 │                                                                     │                            │
21 │                                                                     │                            │
22 │                                                                     │                            │
23 ├─[Message]───────────────────────────────────────────────────────────┼─[House]────────────────────┤
24 │Messages go here                                                     │House: $1000                │25 │                                                                     │                            │
26 │                                                                     │                            │
27 │                                                                     │                            │
28 │                                                                     ├─[Player]───────────────────┤
29 │                                                                     │Player: $500                │
30 │                                                                     │                            │
31 │                                                                     │                            │
32 │                                                                     │                            │
33 │                                                                     │                            │
34 │                                                                     │                            │
35 │                                                                     ├─[Command]──────────────────┤
36 │                                                                     │Command>                    │
37 └─────────────────────────────────────────────────────────────────────┴────────────────────────────┘
38  Press any key to exit...

*/

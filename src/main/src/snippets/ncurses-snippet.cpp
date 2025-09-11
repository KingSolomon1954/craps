#include <ncurses.h>
#include <string>

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

int main()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0); // hide cursor

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
    
    curs_set(1); // cursor back on
    mvprintw(Layout::total_rows, 0, "Press any key to exit...");
    refresh();

    getch();
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

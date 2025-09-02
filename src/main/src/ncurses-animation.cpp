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
    static constexpr int row_player  = 29;
    
    static constexpr int animation_rows = 24;
    static constexpr int animation_cols = 30;
    static constexpr int dice_height = 5;
    static constexpr int dice_width = 7;};

struct Windows
{
    WINDOW *header, *history, *table, *message;
    WINDOW *animation, *house, *player;
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
    mvaddch(L::total_rows-1,L::left_w, ACS_BTEE);

    // Junctions at right border
    mvaddch(L::row_message, L::total_cols-1, ACS_RTEE);
    mvaddch(L::row_player,  L::total_cols-1, ACS_RTEE);

    // Labels
    mvprintw(0,              2, "[Header]");
    mvprintw(L::row_history, 2, "[History]");
    mvprintw(L::row_table,   2, "[Table]");
    mvprintw(L::row_message, 2, "[Message]");

    mvprintw(0,              L::left_w+2, "[Animation]");
    mvprintw(L::row_message, L::left_w+2, "[House]");
    mvprintw(L::row_player,  L::left_w+2, "[Player]");

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
    w.player    = make_subwin(L::row_player,  L::total_rows-1, right_inner_w, Column::Right);

    return w;
}

static void draw_die(WINDOW* w, int top, int left, int value)
{
    // corners
    mvwaddch(w, top + 0, left + 0, ACS_ULCORNER);
    mvwaddch(w, top + 0, left + 6, ACS_URCORNER);
    mvwaddch(w, top + 4, left + 0, ACS_LLCORNER);
    mvwaddch(w, top + 4, left + 6, ACS_LRCORNER);

    // horizontals
    mvwhline(w, top + 0, left + 1, ACS_HLINE, 5);
    mvwhline(w, top + 4, left + 1, ACS_HLINE, 5);

    // verticals
    mvwvline(w, top + 1, left + 0, ACS_VLINE, 3);
    mvwvline(w, top + 1, left + 6, ACS_VLINE, 3);

    for (int r = 1; r <= 3; ++r) {
        mvwhline(w, top + r, left + 1, ' ', 5);
    }

    // interior coordinates
    int rTop = top + 1,  rMid = top + 2,  rBot = top + 3;
    int cL   = left + 2, cM   = left + 3, cR   = left + 4;

    auto pip = [&](int y, int x){
//      mvwaddch(w, y, x, '*' | A_BOLD);
//      mvwaddch(w, y, x, ACS_BULLET);
        mvwaddch(w, y, x, '*');
    };

    wattron(w, A_BOLD);
    switch (value) {
        case 1: pip(rMid, cM); break;
        case 2: pip(rTop, cL); pip(rBot, cR); break;
        case 3: pip(rTop, cL); pip(rMid, cM); pip(rBot, cR); break;
        case 4: pip(rTop, cL); pip(rTop, cR); pip(rBot, cL); pip(rBot, cR); break;
        case 5: pip(rTop, cL); pip(rTop, cR); pip(rMid, cM); pip(rBot, cL); pip(rBot, cR); break;
        case 6: pip(rTop, cL); pip(rTop, cR); pip(rMid, cL); pip(rMid, cR); pip(rBot, cL); pip(rBot, cR); break;
        default: break;
    }
    wattroff(w, A_BOLD);
}

static void animate_roll(WINDOW* anim_win)
{
    static bool seeded = false;
    if (!seeded) { std::srand((unsigned)std::time(nullptr)); seeded = true; }

    int max_y = getmaxy(anim_win);
    int max_x = getmaxx(anim_win);

    const int die_w   = 7;
    const int die_h   = 5;
    const int spacing = 4;

    int total_w = die_w * 2 + spacing;
    if (total_w + 2 > max_x || die_h + 2 > max_y)
    {
        werase(anim_win);
        mvwprintw(anim_win, 1, 1, "Animation window too small");
        wrefresh(anim_win);
        return;
    }

    // center horizontally
    int base_x = (max_x - total_w) / 2;
    if (base_x < 1) base_x = 1;
    int die1_x = base_x;
    int die2_x = base_x + die_w + spacing;

    int start_y   = 1;
    int landing_y = max_y - die_h;

    // Fall with horizontal jitter
    for (int y = start_y; y <= landing_y; ++y)
    {
        werase(anim_win);

        // Wobble, up to ~1/3 screen width shift
        int max_jitter = 6; // try 6..10 for wider swings
        int dx1 = (std::rand() % (2 * max_jitter + 1)) - max_jitter;
        int dx2 = (std::rand() % (2 * max_jitter + 1)) - max_jitter;

        int j1 = die1_x + dx1;
        int j2 = die2_x + dx2;

        // clamp so dice stay inside but can butt the borders
        if (j1 < 1) j1 = 1;
        if (j2 < 1) j2 = 1;
        if (j1 + die_w > max_x - 1) j1 = (max_x - 1) - die_w;
        if (j2 + die_w > max_x - 1) j2 = (max_x - 1) - die_w;

        // spin faces while falling
        int v1 = (std::rand() % 6) + 1;
        int v2 = (std::rand() % 6) + 1;

        draw_die(anim_win, y, j1, v1);
        draw_die(anim_win, y, j2, v2);

        wrefresh(anim_win);
        napms(28);
    }

    // Final rolled values
    int final1 = (std::rand() % 6) + 1;
    int final2 = (std::rand() % 6) + 1;

    // Settle
    int amplitudes[] = {2, 1, 1, 0};
    for (int a : amplitudes)
    {
        int dx1 = (a == 0) ? 0 : ((std::rand() % (2 * a + 1)) - a);
        int dx2 = (a == 0) ? 0 : ((std::rand() % (2 * a + 1)) - a);
        int j1 = die1_x + dx1;
        int j2 = die2_x + dx2;

        if (j1 < 1) j1 = 1;
        if (j2 < 1) j2 = 1;
        if (j1 + die_w > max_x - 1) j1 = (max_x - 1) - die_w;
        if (j2 + die_w > max_x - 1) j2 = (max_x - 1) - die_w;

        werase(anim_win);

        draw_die(anim_win, landing_y, j1, final1);
        draw_die(anim_win, landing_y, j2, final2);

        wrefresh(anim_win);
        napms(55);
    }

    // Final clean render (no jitter)
    werase(anim_win);
    std::string msg = "You rolled " + std::to_string(final1) + " + "
                      + std::to_string(final2) + " = "
                      + std::to_string(final1 + final2);
    mvwprintw(anim_win, 0, (max_x - msg.size()) / 2, "%s", msg.c_str());

    // Draw dice
    draw_die(anim_win, landing_y, die1_x, final1);
    draw_die(anim_win, landing_y, die2_x, final2);
    wrefresh(anim_win);
}

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

    // Refresh all subwindows
    wnoutrefresh(w.header);
    wnoutrefresh(w.history);
    wnoutrefresh(w.table);
    wnoutrefresh(w.message);
    wnoutrefresh(w.animation);
    wnoutrefresh(w.house);
    wnoutrefresh(w.player);
    doupdate();
    
    int ch;
    while ((ch = getch()) != 'q')
    {
        if (ch == 'r')
        {
            animate_roll(w.animation);
        }
    }

    // delwin(w.animation);
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
24 │Messages go here                                                     │House: $1000                │
25 │                                                                     │                            │
26 │                                                                     │                            │
27 │                                                                     │                            │
28 │                                                                     │                            │
29 │                                                                     ├─[Player]───────────────────┤
30 │                                                                     │Player: $500                │
31 │                                                                     │                            │
32 │                                                                     │                            │
33 │                                                                     │                            │
34 │                                                                     │                            │
35 │                                                                     │                            │
36 │                                                                     │                            │
37 └─────────────────────────────────────────────────────────────────────┴────────────────────────────┘

*/

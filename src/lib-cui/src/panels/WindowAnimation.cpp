//----------------------------------------------------------------
//
// File: WindowAnimation.cpp
//
//----------------------------------------------------------------

#include <cui/panels/WindowAnimation.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <cui/CuiMain.h>
#include <cui/CuiUtils.h>
#include <controller/CrapsReaders.h>
#include <wchar.h>
#include <cwchar>

using namespace Cui;

//----------------------------------------------------------------

WindowAnimation::WindowAnimation()
    : PanelBase("WindowAnimation")
{
    newWindow(Layout::animationHeight,           // In base class
              Layout::animationWidth,
              Layout::animationTopRow,
              Layout::animationLeftCol);
}

//----------------------------------------------------------------

WindowAnimation&
WindowAnimation::instance()
{
    static WindowAnimation wa;
    return wa;
}

//----------------------------------------------------------------

void
WindowAnimation::draw()
{
    werase(pWin_);

    // drawExternalJunctions(); // N/A
    // drawInternalBorders();   // N/A
    // drawStaticContent();     // N/A
    std::string msg = "Animation Window "; +
    mvwprintw(pWin_, 0, (Layout::animationWidth - msg.size()) / 2, "%s", msg.c_str());

    animate();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

void
WindowAnimation::drawDie(int top, int left, int value)
{
    // corners
    mvwaddch(pWin_, top + 0, left + 0, ACS_ULCORNER);
    mvwaddch(pWin_, top + 0, left + 6, ACS_URCORNER);
    mvwaddch(pWin_, top + 4, left + 0, ACS_LLCORNER);
    mvwaddch(pWin_, top + 4, left + 6, ACS_LRCORNER);

    // horizontals
    mvwhline(pWin_, top + 0, left + 1, ACS_HLINE, 5);
    mvwhline(pWin_, top + 4, left + 1, ACS_HLINE, 5);

    // verticals
    mvwvline(pWin_, top + 1, left + 0, ACS_VLINE, 3);
    mvwvline(pWin_, top + 1, left + 6, ACS_VLINE, 3);

    for (int r = 1; r <= 3; ++r) {
        mvwhline(pWin_, top + r, left + 1, ' ', 5);
    }

    // interior coordinates
    int rTop = top + 1,  rMid = top + 2,  rBot = top + 3;
    int cL   = left + 2, cM   = left + 3, cR   = left + 4;

    auto pip = [&](int y, int x)
    {
        if (CuiMain::instance().useUnicodePips())
        {
            mvwaddwstr(pWin_, y, x, L"●");
        }
        else
        {
            mvwaddch(pWin_, y, x, '*');
        }
    };

    wattron(pWin_, A_BOLD);
    switch (value)
    {
        case 1: pip(rMid, cM); break;
        case 2: pip(rTop, cL); pip(rBot, cR); break;
        case 3: pip(rTop, cL); pip(rMid, cM); pip(rBot, cR); break;
        case 4: pip(rTop, cL); pip(rTop, cR); pip(rBot, cL); pip(rBot, cR); break;
        case 5: pip(rTop, cL); pip(rTop, cR); pip(rMid, cM); pip(rBot, cL); pip(rBot, cR); break;
        case 6: pip(rTop, cL); pip(rTop, cR); pip(rMid, cL); pip(rMid, cR); pip(rBot, cL); pip(rBot, cR); break;
        default: break;
    }
    wattroff(pWin_, A_BOLD);
}

//----------------------------------------------------------------

void
WindowAnimation::animate()
{
    using L = Layout;
    
    static bool seeded = false;
    if (!seeded) { std::srand((unsigned)std::time(nullptr)); seeded = true; }

    const int die_w   = 7;
    const int die_h   = 5;
    const int spacing = 4;

    int total_w = die_w * 2 + spacing;
    
    // center horizontally
    int base_x = (L::animationWidth - total_w) / 2;
    if (base_x < 1) base_x = 1;
    int die1_x = base_x;
    int die2_x = base_x + die_w + spacing;

    int start_y   = 1;
    int landing_y = L::animationHeight - die_h;

    // Fall with horizontal jitter
    for (int y = start_y; y <= landing_y; ++y)
    {
        werase(pWin_);

        // Wobble, up to ~1/3 screen width shift
        int max_jitter = 6; // try 6..10 for wider swings
        int dx1 = (std::rand() % (2 * max_jitter + 1)) - max_jitter;
        int dx2 = (std::rand() % (2 * max_jitter + 1)) - max_jitter;

        int j1 = die1_x + dx1;
        int j2 = die2_x + dx2;

        // clamp so dice stay inside but can butt the borders
        if (j1 < 1) j1 = 1;
        if (j2 < 1) j2 = 1;
        if (j1 + die_w > L::animationWidth - 1) j1 = (L::animationWidth - 1) - die_w;
        if (j2 + die_w > L::animationWidth - 1) j2 = (L::animationWidth - 1) - die_w;

        // spin faces while falling
        int v1 = (std::rand() % 6) + 1;
        int v2 = (std::rand() % 6) + 1;

        drawDie(y, j1, v1);
        drawDie(y, j2, v2);

        wrefresh(pWin_);
        napms(90);  // napms(28);
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
        if (j1 + die_w > L::animationWidth - 1) j1 = (L::animationWidth - 1) - die_w;
        if (j2 + die_w > L::animationWidth - 1) j2 = (L::animationWidth - 1) - die_w;

        werase(pWin_);

        drawDie(landing_y, j1, final1);
        drawDie(landing_y, j2, final2);

        wrefresh(pWin_);
        napms(55);
    }

    // Final clean render (no jitter)
    werase(pWin_);
    
    int total = final1 + final2;

    std::wstring msg =
        L"\U0001F3B2 Roll " + std::to_wstring(total)  +
        L"("                + std::to_wstring(final1) +
        L","                + std::to_wstring(final2) +
        L") \U0001F3B2";

    int msgW = 0;
    for (wchar_t ch : msg)
    {
        int w = wcwidth(ch);
        if (w > 0) msgW += w;
    }
    mvwaddwstr(pWin_, 0, (L::animationWidth - msgW) / 2, msg.c_str());

    // Draw dice
    drawDie(landing_y, die1_x, final1);
    drawDie(landing_y, die2_x, final2);
    wrefresh(pWin_);
}

//----------------------------------------------------------------

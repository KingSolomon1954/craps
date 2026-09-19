//----------------------------------------------------------------
//
// File: WindowAnimation.cpp
//
//----------------------------------------------------------------

#include <cui/panels/WindowAnimation.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <cui/CuiMain.h>
#include <cui/CuiThread.h>
#include <cui/CuiUtils.h>
#include <cui/WorkOrder.h>
#include <wchar.h>

using namespace Cui;

//----------------------------------------------------------------

WindowAnimation::WindowAnimation()
    : PanelBase("WindowAnimation")
{
    newWindow(Layout::animationHeight,           // In base class
              Layout::animationWidth,
              Layout::animationTopRow,
              Layout::animationLeftCol);

    // Timer callback to render animation frames
    timerId_ = Gen::TimerManager::instance().createTimer(
            [&]
            {
                enqueueDraw();
            }
        ); // Not armed yet
    std::srand((unsigned)std::time(nullptr));
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
WindowAnimation::onDiceThrowStart()
{
    state_ = AnimationState::Animating;
    startAnimation();
}

//----------------------------------------------------------------

void
WindowAnimation::startAnimation()
{
    // Start the animation timer, repeats every "n" mils.
    Gen::TimerManager::instance().armTimer(
        timerId_, std::chrono::milliseconds(90), true);
}

//----------------------------------------------------------------
//
// Called by animation rendering when dice finally lands at 
// final resting spot.
//
void
WindowAnimation::stopAnimation()
{
    Gen::TimerManager::instance().cancelTimer(timerId_);
    state_ = AnimationState::ShowingRoll;

    // One last draw() workorder. // Will draw() final
    // dice in the ShowingRoll state
    enqueueDraw();
}

//----------------------------------------------------------------

void
WindowAnimation::onDiceNewValue(int d1, int d2, int rollCount)
{
    lastRoll_.d1 = d1;
    lastRoll_.d2 = d2;
    lastRoll_.value = d1 + d2;
    lastRoll_.rollCount = rollCount;

    // These values will be used later when the animation finishes.
}

//----------------------------------------------------------------

void
WindowAnimation::enqueueDraw()
{
    WorkOrderSurface wo{.type = SurfaceType::Draw, .pSurface = this};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
WindowAnimation::draw()
{
    werase(pWin_);
    
    switch(state_)
    {
    case AnimationState::NoRoll:      drawNoRoll();         break;
    case AnimationState::Animating:   drawAnimationFrame(); break;
    case AnimationState::ShowingRoll: drawFinalDice();      break;
    }

    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// Display this on top line of animation window:  🎲 Rolling... 🎲
// 
void
WindowAnimation::drawBanner()
{
    std::wstring left  = L"\U0001F3B2 ";
    std::wstring right = L" \U0001F3B2";
    std::wstring middle;
    
    if (state_ == AnimationState::NoRoll)
    {
        middle = L"Roll Waiting";
    }

    if (state_ == AnimationState::Animating)
    {
        middle = L"Rolling ...";
    }

    if (state_ == AnimationState::ShowingRoll)
    {
        middle = std::to_wstring(lastRoll_.value)     +
                 L"(" + std::to_wstring(lastRoll_.d1) +
                 L"," + std::to_wstring(lastRoll_.d2);
    }

    std::wstring msg = left + middle + right;

    int msgW = 0;
    for (wchar_t ch : msg)
    {
        int w = wcwidth(ch);
        if (w > 0) msgW += w;
    }
    mvwaddwstr(pWin_, 0, (Layout::animationWidth - msgW) / 2, msg.c_str());
}

//----------------------------------------------------------------

void
WindowAnimation::drawNoRoll()
{
    // Leave window blank. Initial state. There's no dice to show.
    drawBanner();
}

//----------------------------------------------------------------
//
// Draws one animation frame.
//
// When the animation frame timer triggers, it enqueues a
// draw workorder to the CuiThread, which the SurfaceManager
// fields and forwards to here.
//
void
WindowAnimation::drawAnimationFrame()
{
    drawBanner();
    animateFrame();  // Render a new animation frame
}

//----------------------------------------------------------------

void
WindowAnimation::drawFinalDice()
{
#if 0    
    int landing_y = L::animationHeight - die_h;

    drawDie(landing_y, j1, final1);
    drawDie(landing_y, j2, final2);
#endif

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
    int rTop = top  + 1, rMid = top  + 2, rBot = top  + 3;
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
WindowAnimation::animateFrame()
{
    using L = Layout;
    
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

// TODO flatten
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

//  TODO Flatten, Move out of here
    
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

        drawDie(landing_y, j1, lastRoll_.d1);
        drawDie(landing_y, j2, lastRoll_.d2);

        wrefresh(pWin_);
        napms(55);
    }

    // call stopAnimation() when dice reach bottom of the window

    // Move out of here
    // Final clean render (no jitter)
    werase(pWin_);
    
    // Draw dice
    drawDie(landing_y, die1_x, lastRoll_.d1);
    drawDie(landing_y, die2_x, lastRoll_.d2);
    wrefresh(pWin_);
}

//----------------------------------------------------------------

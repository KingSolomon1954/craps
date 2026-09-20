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
    state_          = AnimationState::Animating;
    animationPhase_ = AnimationPhase::Falling;
    startAnimation();
}

//----------------------------------------------------------------

void
WindowAnimation::startAnimation()
{
    // Start the animation timer, repeats every "n" mils.
    Gen::TimerManager::instance().armTimer(
        timerId_, std::chrono::milliseconds(90), true);
    animationY_ = 1;
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
    case AnimationState::ZeroRoll:    drawZeroRoll();    break;
    case AnimationState::Animating:   drawAnimation();   break;
    case AnimationState::ShowingRoll: drawShowingRoll(); break;
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
    
    if (state_ == AnimationState::ZeroRoll)
    {
        middle = L"Roll Waiting";
    }

    if (state_ == AnimationState::Animating)
    {
        if (animationPhase_ == AnimationPhase::Done)
        {
            middle = middleFinishedRoll();
        }
        else
        {
            middle = L"Rolling ...";
        }
    }

    if (state_ == AnimationState::ShowingRoll)
    {
        middle = middleFinishedRoll();
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

std::wstring
WindowAnimation::middleFinishedRoll()
{
    return L"Rolled: " + std::to_wstring(lastRoll_.value) +
           L"("        + std::to_wstring(lastRoll_.d1)    +
           L","        + std::to_wstring(lastRoll_.d2)    +
           L")";
}

//----------------------------------------------------------------

void
WindowAnimation::drawZeroRoll()
{
    // Initial state. There's no dice to show.
    drawBanner();
}

//----------------------------------------------------------------

void
WindowAnimation::drawShowingRoll()
{
    drawBanner();
    drawDie(LandingY, Die1X, lastRoll_.d1);
    drawDie(LandingY, Die2X, lastRoll_.d2);
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
WindowAnimation::drawAnimation()
{
    drawBanner();
    
    switch (animationPhase_)
    {
    case AnimationPhase::Falling:
        renderFallingFrame();
        break;

    case AnimationPhase::Settling:
        renderSettlingFrame();
        break;

    case AnimationPhase::Done:
        renderFinalFrame();
        stopAnimation();
        break;
    }
}

//----------------------------------------------------------------

void
WindowAnimation::renderFallingFrame()
{
    int j1 = clampDieX(Die1X + randomJitter(6));
    int j2 = j2Helper(j1);
    
    int v1 = randomDieValue();
    int v2 = randomDieValue();

    drawDie(animationY_, j1, v1);
    drawDie(animationY_, j2, v2);

    ++animationY_;

    if (animationY_ > LandingY)
    {
        animationPhase_ = AnimationPhase::Settling;
        settleIndex_ = 0;
    }
}

//----------------------------------------------------------------

int
WindowAnimation::j2Helper(int j1)
{
    int j2;
    do
    {
        j2 = clampDieX(Die2X + randomJitter(6));
    }
    while (j2 < j1 + DieWidth);
    
    return j2;
}

//----------------------------------------------------------------

void
WindowAnimation::renderSettlingFrame()
{
    static constexpr std::array<int, 4> SettleAmplitudes{2, 1, 1, 0};

    int amplitude = SettleAmplitudes[settleIndex_];

    int j1 = clampDieX(Die1X + randomJitter(amplitude));
    int j2 = clampDieX(Die2X + randomJitter(amplitude));

    drawDie(LandingY, j1, lastRoll_.d1);
    drawDie(LandingY, j2, lastRoll_.d2);

    ++settleIndex_;

    if (settleIndex_ >= SettleAmplitudes.size())
    {
        animationPhase_ = AnimationPhase::Done;
    }
}

//----------------------------------------------------------------

void
WindowAnimation::renderFinalFrame()
{
    drawDie(LandingY, Die1X, lastRoll_.d1);
    drawDie(LandingY, Die2X, lastRoll_.d2);
}

//----------------------------------------------------------------

int
WindowAnimation::randomJitter(int max)
{
    return (std::rand() % (2 * max + 1)) - max;
}

//----------------------------------------------------------------

int
WindowAnimation::randomDieValue()
{
    return (std::rand() % 6) + 1;
}

//----------------------------------------------------------------
//
// Clamp so dice stay inside but can butt the borders
//
int
WindowAnimation::clampDieX(int j)
{
    if (j < 1) j = 1;
    if (j + DieWidth > Layout::animationWidth - 1)
    {
        j = (Layout::animationWidth - 1) - DieWidth;
    }
    return j;
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

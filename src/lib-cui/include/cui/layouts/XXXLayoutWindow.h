// Throw away later


class LayoutWindow
{
public:
    static void init(WINDOW* pWin)
    {
        if (pWin_)
        {
            throw std::logic_error("LayoutWindow already initialized");
        }
        pWin_ = pWin;
    }

    static LayoutWindow& instance()
    {
        if (!pWin_)
        {
            throw std::logic_error("LayoutWindow::init() must be called first");
        }
        static LayoutWindow layoutWin;   // only reached, hence only constructed, if pWin_ is set
        return layoutWin;
    }

    LayoutWindow(const LayoutWindow&) = delete;
    LayoutWindow& operator=(const LayoutWindow&) = delete;

private:
    LayoutWindow()
    {
        // pWin_ is guaranteed non-null here
        ...
    }

    static WINDOW* pWin_ = nullptr;
    
    // May need to use instead C++17 give trouble
    // static inline WINDOW* pWin_ = nullptr;
};

// .cpp
WINDOW* LayoutWindow::pWin_ = nullptr;



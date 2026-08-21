


class ConsoleManager : public ViewInterface
{
private:
    std::vector<ViewSurface*> stack_;

public:
    void setSurface(ViewSurface*);
    void pushSurface(ViewSurface*);
    void popSurface();

    void redraw();
    void handleKey(int ch);
};



/* The nice thing is that ConsoleManager doesn't care. */

/* It simply does: */

/* surface->handleKey(ch); */

/* and: */

/* surface->draw(); */

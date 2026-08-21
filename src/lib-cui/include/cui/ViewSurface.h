



class ViewSurface
{
public:
    virtual ~ViewSurface() = default;

    virtual void onAttach()  {}
    virtual void onDetach()  {}
    virtual void onPause()   {}
    virtual void onResume()  {}

    virtual void draw() = 0;
    virtual void handleKey(int ch) = 0;
};

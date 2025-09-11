#include <cui/ConsoleView.h>
#include <cui/DialogAmountEntry.h>

int main() {
    Cui::ConsoleView view;
    view.init(); // sets up ncurses
    
    Cui::DialogAmountEntry dialog(view);
    dialog.clearState();
    dialog.setTitle("Test Amount Entry");
    view.pushScreen(&dialog);
    view.run();  // your normal event loop
    
    view.shutdown();
}

#include <iostream>
#include <unistd.h>
#include <pbar.hh>

void test_text () {
    int flags = pbar::SHOW_VALUE | pbar::SHOW_COUNT;
    pbar::TextProgressBar pb(100,flags);
    for (int i = 0; i < 100; ++i) { pb.update (); usleep (50000); }
}

void test_color () {
    int flags = pbar::SHOW_VALUE | pbar::SHOW_COUNT;
    pbar::ColorProgressBar pb(100,flags);
    for (int i = 0; i < 100; ++i) { pb.update (); usleep (50000); }
}

#ifdef PBAR_GUI
void test_gui () {
    pbar::GraphicalProgressBar pb(100);
    for (int i = 0; i < 100; ++i) { pb.update (); usleep (50000); }
}
#endif

int main () {

    std::cout << "Testing the text progress bar: " << std::endl;
    test_text ();

    std::cout << "Testing the color progress bar: " << std::endl;
    test_color ();

#ifdef PBAR_GUI
    std::cout << "Testing the gui progress bar: " << std::endl;
    test_gui ();
#endif

    std::cout << "Tests complete" << std::endl;
    return 0;
}

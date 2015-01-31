#include <iostream>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <pbar.hh>

/* set an arbitrary string for the prefix */
std::string my_prefix (::size_t max) {
    static int x = 0;
    std::stringstream ss;
    ++x;
    ss << "1/10 * " << (x / 10);
    return ss.str ();
}

/* place the string '(db)' randomly within the progress meter */
void my_value (std::string &p) {
    static int round = 0;
    static ::size_t pos = 0;
    std::string custom = "(db)";
    ::size_t clen = custom.length (), plen = p.length ();
    ::size_t q = plen / 4;
    if (0 == round % 25)
        pos = q + rand () % ((plen - clen) / 2);
    ++round;
    std::string s = p.substr (0, pos) 
        + custom 
        + p.substr (pos + clen, plen - (pos + clen));
    p = s;
}
    
void test_text () {
    int flags = pbar::SHOW_VALUE | pbar::SHOW_PREFIX;
    pbar::TextProgressBar pb(100,flags);
    for (int i = 0; i < 100; ++i) { pb.update (); usleep (50000); }
}

void test_callback () {
    int flags = pbar::SHOW_VALUE | pbar::SHOW_PREFIX;
    pbar::TextProgressBar pb(100,flags);
    pb.set_prefix_callback (&my_prefix);
    pb.set_value_callback (&my_value);
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

    std::cout << "Testing the custom callbacks: " << std::endl;
    test_callback ();

#ifdef PBAR_GUI
    std::cout << "Testing the gui progress bar: " << std::endl;
    test_gui ();
#endif

    std::cout << "Tests complete" << std::endl;
    return 0;
}

/*
    Copyright (C) 2011  Jesse Brown

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PROGRESS_BAR_HH__
#define PROGRESS_BAR_HH__

#include <string>
#include <iostream>
#include <vector>

extern "C" {
#ifdef PBAR_GUI
#include <gtk/gtk.h>
#endif // PBAR_GUI
#include <pthread.h>
}

namespace pbar {

    /*
     * Color/Attribute character combinations
     */
    static const char ColorBlue[] = { 0x1b, '[', '3', '4', 'm', 0 };
    static const char ColorRed[] = { 0x1b, '[', '3', '1', 'm', 0 };
    static const char AttrBold[] = { 0x1b, '[', '1', 'm', 0 };
    static const char AttrClear[] = { 0x1b, '[', '0', 'm', 0 };

    /*
     * Output configuration flags
     */
    enum { 
        SHOW_VALUE = (0x1 << 0), // show the % complete
        SHOW_PREFIX = (0x1 << 1)  // show the N/M status
    };
}

namespace pbar {

/**
 * Callback function type (used to build the prefix string)
 * @param w The overall width of the screen. 
 * @return A string (not longer than w) that will precede the progress bar
 */
typedef std::string (*prefix_fun)(::size_t w);

/**
 * Callback function type (used to build the progress meter)
 * @param s The progress bar string (without formatting)
 * @note This function should overwrite the string passed in as necessary. Simply
 * returning the input string will show the progress as [***      ] without
 * additional text added.
 */
typedef void (*value_fun)(std::string& s);

/**
 * Generic ProgressBar class
 * Provides the common interface for all variations of ProgressBar
 * Prevents copying to avoid problems with multiple writers to a 
 * shared output device
 */
class ProgressBar {
protected:

    ::size_t capacity_, value_;

private:

    // Prevent copying of progress bars
    ProgressBar (const ProgressBar&);
    ProgressBar& operator= (const ProgressBar&);

public:

    ProgressBar (::size_t cap) : capacity_(cap), value_(0) {}

    virtual ~ProgressBar () {}

    /*
     * Update the current count of the progress bar by n
     */
    virtual void update (::size_t n = 1) = 0;
};

/**
 * Textual progress bar that uses stars to mark progress along the screen
 */
class TextProgressBar : ProgressBar {

    prefix_fun cb_prefix_;
    value_fun cb_value_;

    /*
     * Format configuration. 
     * Defines whether current value and/or percent complete
     * is displayed to the user
     * The bit fields relate directly to enum values SHOW_*
     */
    struct Config {
        unsigned show_value:1; // Show XX.X% 
        unsigned show_prefix:1; // Show N/M
        Config (int opts) : show_value(0), show_prefix(0) {
           if (opts & SHOW_VALUE) { show_value = 1; }
           if (opts & SHOW_PREFIX) { show_prefix = 1; }
        }
    };

    std::ostream& display;  // output device
    Config config;

    /*
     * Generate a string (of provided width) that represents
     * the percentage complete so far 
     */
    std::string progress (::size_t);

    /*
     * Determine the current screen width, build the output string,
     * refresh the display
     */
    void show ();

    /*
     * Given a string from TextProgressBar::progress, overwrite
     * a portion of the string with the percentage complete (centered)
     */
    void format_value (std::string& s);

    /*
     * Create an N/M string representing the total value of the progress bar
     * The provided value is a maximum width allowed for the resulting string
     */
    std::string format_count (::size_t);

    /*
     * Clear the current display
     */
    void clear ();

public:

    TextProgressBar (std::ostream& os, ::size_t cap, int opts = SHOW_VALUE) :
        ProgressBar(cap), 
        cb_prefix_(0), cb_value_(0),
        display(os), config(opts) {}

    TextProgressBar (::size_t capacity, int opts = SHOW_VALUE) :
        ProgressBar(capacity), 
        cb_prefix_(0), cb_value_(0),
        display(std::cerr), config(opts) {}

    /**
     * callback used to create the prefix
     */
    inline void set_prefix_callback (prefix_fun fun) { cb_prefix_ = fun; }

    /**
     * callback used to format the actual progress bar
     */
    inline void set_value_callback (value_fun fun) { cb_value_ = fun; }

    ~TextProgressBar () { clear (); }

    virtual void update (::size_t n = 1);
};

#ifdef PBAR_GUI
/**
 * Provide a gtk+-based GUI version of the ProgressBar.
 * Using gtk facilities, attempt to strip as much window dressing
 * as possible from the progress bar object
 * The gtk environment is hosted in a separate thread allowing the
 * caller to operate without direct gtk interaction
 */
class GraphicalProgressBar : ProgressBar {

    /*
     * 'handle' to the gtk progress bar widget and its timer
     */
    struct Progress {
        GtkWidget * pb;
        int timer;
        ::size_t val, max;
        Progress () : pb(0), timer(0), val(0), max(0) {}
    };

    /*
     * This object provides a way to pass any state to the gtk thread
     * when it is spawned
     */
    struct ThreadArg {
        int * argc;
        char *** argv;
        Progress * p;
        ThreadArg () : argc(0), argv(0), p(0) {}
    };

    ThreadArg ta_;
    Progress p_;
    pthread_t thread_;

    inline void create_thread (ThreadArg * ta) {
        pthread_create (&thread_, NULL, GraphicalProgressBar::run_gtk, ta);
    }

    /*
     * This method is what the thread runs to display the progress bar
     */
    static void * run_gtk (void *);

    /*
     * Controls the period at which the gtk widget checks for updates
     */
    static gboolean update_timer (gpointer data);

public:

    GraphicalProgressBar (::size_t capacity) : ProgressBar(capacity) {
        /*
         * Assign the handle to the widget and our capacity, then spawn thread 
         */
        ta_.p = &p_;
        p_.max = capacity;
        create_thread (&ta_);
    }

    ~GraphicalProgressBar () { gtk_main_quit (); }

    virtual void update (::size_t n = 1) {
        p_.val += n;
        if (p_.val > p_.max) {
            p_.val = p_.max;
        }
    }

};

#endif // PBAR_GUI

} // namespace pbar


#endif //PROGRESS_BAR_HH__

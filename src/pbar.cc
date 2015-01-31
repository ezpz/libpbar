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

#include <sstream>
#include <cstdio>
#include <pbar.hh>

extern "C" {
#include <sys/ioctl.h>
#include <termios.h>
}

namespace pbar {

std::string
TextProgressBar::format_count (::size_t width) {
    std::stringstream ss("");
    ss << '(' << value_ << '/' << capacity_ << ')';

    // Trim to fit if we used up all the room
    if (ss.str ().length () > width) {
        ss.str ("");
        ss << value_;
        
        // If still too long, just elide the prefix
        if (ss.str ().length () > width) {
            ss.str("");
        }
    }

    return ss.str ();
}

void
TextProgressBar::format_value (std::string& s) {
    double v = 100 * static_cast< double >(value_) / 
                static_cast< double >(capacity_);
    ::size_t width = s.length ();
    std::stringstream ss;
    ss.setf (std::ios::fixed, std::ios::floatfield);
    ss.precision (1);
    ss << '[' << v << "%]";
    std::string p = ss.str ();
    // Forget it if we dont have the room
    if (p.length () < width) {
        // Find center, adjusteded for text
        ::size_t c = width / 2 - p.length () / 2;
        s.replace (c, p.length (), p);
    }
}

std::string
TextProgressBar::progress (::size_t width) {
    std::string s(width, ' ');
    double x = 
        static_cast< double >(value_) / static_cast< double >(capacity_);
    int stars = static_cast< int >(x * width);
    s.replace (0, stars, stars, '*');
    return s;
}

void
TextProgressBar::clear () {
    struct winsize ws;
    ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws);
    ::size_t width = static_cast< ::size_t >(ws.ws_col);
    std::string blank(width + 2, ' ');
    blank[0] = '\r';
    blank[width + 1] = '\r';
    display << blank << std::flush;
}

void
TextProgressBar::show () {
    struct winsize ws;
    ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws);
    ::size_t width = static_cast< ::size_t >(ws.ws_col);
    std::stringstream ss;

    ss << '\r';

    if (config.show_prefix) {
        std::string s = cb_prefix_ ? cb_prefix_ (width) : format_count (width);
        ss << s;
        width -= s.length (); // format_count ensures this will be valid
    }

    /*
     * If there is not enough room for at least "[*]" then just show prefix
     */
    if (width < 3) {
        display << ss.str () << std::flush;
        return;
    }

    ss << '[';
    std::string s = progress (width - 2);
    if (config.show_value) {
        ::size_t orig_width = s.length ();
        cb_value_ ? cb_value_ (s) : format_value (s);
        if (s.length () > orig_width) {
            /* trim to required length if callback did not obey width */
            s = s.substr (0, orig_width);
        }
    }
    ss << s << ']';

    display << ss.str () << std::flush;
}

void
TextProgressBar::update (::size_t n) {
    value_ += n;
    if (value_ > capacity_) {
        value_ = capacity_;
    }
    show ();
}
    
void * 
GraphicalProgressBar::run_gtk (void * arg) {
    int h = -1, w = 400;
    ThreadArg * ta = (ThreadArg *)arg;
    GtkWidget * window = 0;
    gtk_init (ta->argc, ta->argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(window), "ProgressBar");
    // attempt to remove window dressings
    gtk_window_set_decorated (GTK_WINDOW(window), FALSE);
    // attempt to set the window opacity
    if (gtk_widget_is_composited (window)) {
        gtk_window_set_opacity (GTK_WINDOW(window), 0.5);
    } 
    gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    ta->p->pb = gtk_progress_bar_new ();
    gtk_widget_set_size_request (ta->p->pb, w, h);
    ta->p->timer = 
        g_timeout_add (100, GraphicalProgressBar::update_timer, ta->p);
    gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET(ta->p->pb));
    gtk_progress_bar_set_text ((GtkProgressBar *)ta->p->pb, "0%");
    gtk_container_set_border_width (GTK_CONTAINER (window), 0);
    gtk_widget_show (ta->p->pb);
    gtk_widget_show (window);
    gtk_main ();
    return NULL;
}

gboolean 
GraphicalProgressBar::update_timer (gpointer data) {
    GraphicalProgressBar::Progress * pdata = 
        (GraphicalProgressBar::Progress *)data;
    double d = pdata->val / (double)pdata->max;
    char msg[255] = {0};
    sprintf (msg, "%0.1f%%", 100 * d);
    gtk_progress_bar_set_text ((GtkProgressBar *)pdata->pb, msg);
    gtk_progress_bar_set_fraction ((GtkProgressBar *)pdata->pb, d);
    return TRUE;
}

} // namespace pbar

lib_LTLIBRARIES = libpbar.la
libpbar_la_SOURCES = pbar.cc 
# Force the PBAR_GUI macro here since we require GTK during the 
# library build. Though, the user is free to turn this on/off during
# local builds to avoid having to call in gtk-specific files
libpbar_la_CPPFLAGS = -DPBAR_GUI -W -Wall -Wextra -ansi -I../include
library_includedir=$(includedir)
library_include_HEADERS = ../include/pbar.hh 

# libpbar
A simple progress bar library providing several default implementations including a basic text, colored text, and GTK+ GUI version.

## Description
The library provides a very simple interface the includes only an `update` method which adds an amount to the surrent progress (default: 1 unit).

```c++
class ProgressBar {
public:
   ProgressBar (::size_t cap);
   virtual void update (::size_t n = 1) = 0;
};
```

## Rationale
For examples and rationale, see [this bog post](http://uu-kk.blogspot.com/2011/07/progress.html)

## Building
The standard `./configure && make && make install` setup is provided. Although, tput and GTK+ >= 2.0 are required.

## Using
Each example here referes to the `src/test.cc` file included with the library.

Building __without__ GTK+ support: `g++ test.cc -lpbar`

Building __with__ GTK+ support: `g++ -DPBAR_GUI \`pkg-config gtk+-2.0 --cflags\` test.cc -lpbar`

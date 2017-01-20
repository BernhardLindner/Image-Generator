# SDL Viewer #

Provides a live view of the Image Generator's output in an SDL window

## Prerequisites ##

* libSDL2
* `pkg-config` for the Makefile
* Makefile uses `clang`

## Getting Started ##

1. Build by `make`
2. Run an image generator 
3. Enjoy live output using `image_viewer`

## ToDo ##

* `valgrind` shows some memory leaks concerning SDL. Is this our fault, SDL's or X11's?

## Contributors ##

* Based on Bernhard Lindner's Image Writer
* SDLified by Christian Fibich

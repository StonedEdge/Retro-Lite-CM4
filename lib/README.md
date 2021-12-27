# Dispmanx

There are a number of APIs available for the Raspberry Pi that can make use
of the computers GPU. These include OpenMAX, Open GL ES(1 and 2) and OpenVG.
The raspberrypi/firmware repository has short examples for these and other
APIs. They can be found in /opt/vc/src/hello_pi/ on the Raspbian 'wheezy'
image. Among these examples is a program called hello_dispmanx. It is a
very small example of the Dispmanx windowing system. Apart from this example, 
there is very little documentation available for this API. There
are snippets of information on the Raspberry Pi forum, but I have not found
a single place with detailed information on DispmanX.  Hopefully these
programs can be used as a starting point for anyone wanting to make use of
DispmanX.

https://github.com/raspberrypi/firmware/tree/master/opt/vc/src/hello_pi/hello_dispmanx

The programs demonstrate layers with the following types: 4BPP (4 bit
indexed), 8BPP (8 bit indexed), RGB565 (16 bit), RGB888 (24 bit), RGBA16
(16 bit with transparency) and RGBA32 (32 bit with transparency)

## test_pattern

This test pattern should be familiar to anyone who has used the Raspberry
Pi. It is the same four colour square displayed when the Raspberry Pi boots.

## rgb_triangle

Displays a triangle in a layer with red, green and blue gradients starting
at each corner respectively. Blends to grey in the center. Demonstrates
changing size of source and destination rectangles.

## life

Conway's game of life. Demonstrates double buffering.

## worms

The program raspiworms uses a single 16 or 32 bit RGBA layer to display a
number of coloured worms on the screen of the Raspberry Pi.

## pngview

Load a png image file and display it as a Dispmanx layer.

## spriteview

Loads a sprite (png) image file and displays it as an animation.

## game

Demonstrates a seamless background image that can be scolled in any
direction. As well as animated sprites.

## mandelbrot

The famous (in the 1990s) Mandelbrot set.

## radar_sweep

An animation of a 'radar sweep' using 16 bit (rgb) palette animation.

## radar_sweep_alpha

An animation of a 'radar sweep' using 32 bit (rgba) palette animation.

## offscreen

An example of using an offscreen display to resize an image.

## common

Code that may be common to some of the demonstration programs is in this
folder.

## building

If you type make in the top level directory, the make file will build all
the different programs in this repository. Each program has its own make
file, so you can build them individually if you wish.

You will need to install libpng before you build the program. On Raspbian

sudo apt-get install libpng12-dev


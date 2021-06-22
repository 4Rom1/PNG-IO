# PNG-IO
  A simple libpng example program taken from
 * http://zarb.org/~gc/html/libpng.html
 * https://gist.github.com/niw/5963798
  
  Modified by Romain Garnier to read and write from/to
  different intput/outputs of a png file.

  This software may be freely redistributed under the terms
  of the X11 license.
  
 ## Install libpng :
   https://programmersought.com/article/94135545706/
   
   or simply do 
   
 $ sudo apt-get install libpng libpng-dev
 
 compile with
 
 $ g++  pngTest.cpp `libpng-config --ldflags` -o pngTest.out
 
 run with 
 
 $ ./pngTest.out in.png out.png
 
 
 

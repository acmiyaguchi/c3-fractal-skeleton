#include "Mandelbrot.h"
#include <iostream>
#include <cmath>

using namespace std;

namespace{
    double lerp(double v0, double v1, double t) {
        return v0+(v1-v0)*t;
    }
}

void Mandelbrot::gen_fractal()
{
    unsigned int img_width = get_width();
    unsigned int img_height = get_height();
    for(int i = 0; i < img_width; i++) {
        for(int j = 0; j < img_height; j++) {
            //Convert from screen coordinates to complex coordinates
            double x0 = computeRealFromX(i);
            double y0 = computeImaginaryFromY(j);

            //Start z at 0
            double x = 0;
            double y = 0;

            //Iterate until we reach 2 or the max
            int count = 0;
            while(x*x + y*y < 4.0 && count < MAXITER) {
                double xtemp = x*x - y*y + x0;
                y = 2*x*y + y0;
                x = xtemp;
                count++;
            }

           double iter = count;; 
            //Color the pixel
            if (count < MAXITER) {
                double zn = sqrt(x*x + y*y);
                double mu = log2(log2(zn));
                iter = count + 1 - mu;
            }
            //Create the color palatte (0x0, 0xffffff)
            double color1 = iter/MAXITER;
            unsigned int color = ~(unsigned int)(color1*0xffffff);
            //double color2 = color
            setColor(i, j, (color>>16)&0xff, (color>>8)&0xff, color&0xff);
        }
    }
}

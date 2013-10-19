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

            unsigned char rgb[3];

            //Iterate until we reach 2 or the max
            int count = 0;
            while(x*x + y*y < 4.0 && count < MAXITER) {
                double xtemp = x*x - y*y + x0;
                y = 2*x*y + y0;
                x = xtemp;
                count++;
            }

           double iter = count; 
            //Color the pixel
            if (count < MAXITER) {
                double zn = sqrt(x*x + y*y);
                double mu = log2(log2(zn));
                iter = count + 1 - mu;
                //Create the color palatte (0x0, 0xffffff)
                double color = lerp(floor(iter), floor(iter+1), fmod(iter, 1));
                double hue = 0.95 * 15 * (color/count);
                hsvToRgb(hue, 0.8, 1.0, rgb);
                setColor(i, j, rgb[0], rgb[1], rgb[2]);
            }
            else {
                setColor(i, j, 0, 0, 0);
            }
        }
    }
}

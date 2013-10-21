#include "Mandelbrot.h"
#include <iostream>
#include <cmath>

using namespace std;

namespace{
    double lerp(double v0, double v1, double t) {
        return v0+(v1-v0)*t;
    }
    double dist(double count, double x, double y) {
        double zn = sqrt(x + y);
        double mu = log(log(zn))/log(2);
        return count + 1 -mu;
    }
	bool checkBulb(double x, double y2) {
		double q = (x - 0.25)*(x - 0.25) - y2;
		return ((4 * q * (q +(x -0.25))<y2) || (16*((x+1)*(x+1)+y2 < 1)));
	}
}

void Mandelbrot::gen_fractal()
{
    unsigned int img_width = get_width();
    unsigned int img_height = get_height();
	#pragma omp parallel for
	for(int i = 0; i < img_width; i++) {
		double x0 = computeRealFromX(i);
        for(int j = 0; j < img_height; j++) {
            //Convert from screen coordinates to complex coordinates
            double y0 = computeImaginaryFromY(j);

            //Start z at 0
            double x = 0;
            double y = 0;
			double x2 = 0.0;
			double y2 = 0.0;

            unsigned char rgb[3];

            //Iterate until we reach 2 or the max
            int count = 0;
			for(count = 0; count < MAXITER && ((x2 + y2) < (2<<16)); count++) {
                double xtemp = x2 - y2 + x0;
                y = 2*x*y + y0;
                x = xtemp;
				y2 = y*y;
				x2 = x*x;
            }

           double iter;
            //Color the pixel
            if (count == MAXITER) {
				setColor(i, j, 0, 0, 0);
            }
            else {
				 iter = dist((double)count, x2, y2);
                //Create the color palatte (0x0, 0xffffff)
                double color = lerp(floor(iter), floor(iter+1), fmod(iter, 1));
                double hue = 0.95 * 20 * (color/MAXITER);
                hsvToRgb(hue, 0.6, 1.0, rgb);
                setColor(i, j, rgb[0], rgb[1], rgb[2]);
            }
        }
    }
	
}

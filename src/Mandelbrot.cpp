#include "Mandelbrot.h"
#include <iostream>

namespace{
    const int MAX_COUNT = 1000;
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
            while(x*x + y*y < 4.0 && count < MAX_COUNT) {
                double xtemp = x*x - y*y + x0;
                y = 2*x*y + y0;
                x = xtemp;
                count++;
            }
            //std::cout << count << " " << x << " " << y << std::endl;
            //Color the pixel
            if(count == MAX_COUNT) {
                //Its within the set
                setColor(i, j, 0, 0xff, 0);
            }
            else {
                //Not in the set
                setColor(i, j, 0xff, 0xff, 0xff);
            }
        }
    }
}

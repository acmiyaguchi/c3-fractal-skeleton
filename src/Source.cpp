#include "Buddhabrot.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <time.h>

using namespace std;

int is_mandel(double x, double y, unsigned long maxiters) 
{
	double zx = 0,
		zy = 0;
	while (maxiters-- > 0) 
	{
		double zxsq = zx * zx,
			zysq = zy * zy;
		if (zxsq + zysq > 4.0)
			return 0;
		zy = 2 * zx * zy + y;
		zx = zxsq - zysq + x;
	}
	return 1;
}


void Buddhabrot::gen_fractal()
{
	unsigned int image_width=get_width();
	unsigned int image_height=get_height();
	srand(time(NULL));
	// Real (-2.5, 1)
	// Imaginary (-1, 1)

	double* pxarr=(double *)malloc(sizeof(double)*image_width*image_height);
	// Initialize a bucket array (one integer for each pixel) (this is the outer bucket array)
	double maxval = 0;
	unsigned int npoints=image_width*image_height;
	while (--npoints > 0) 
	{
		double cx, cy,
			zx, zy,
			zxsq, zysq;
		do 
		{
			cx = rand()%65535;
			cy = rand()%65535;
		} while (is_mandel(cx, cy, MAXITER));
		zx = cx;
		zy = cy;
		zxsq = zx*zx;
		zysq = zy*zy;

		while (zxsq+zysq<4) 
		{
			size_t pixel_x = (zx + 2.0) * image_width / 3.0,
				pixel_y = (zy + 1.25) * image_height / 2.5;
			if (0 <= pixel_x && pixel_x < image_width &&
				0 <= pixel_y && pixel_y < image_height) 
			{
					unsigned tmp = pxarr[pixel_x*image_height + pixel_y] + 1;
					pxarr[pixel_x*image_height + pixel_y] = tmp;
					if (tmp > maxval) 
						maxval = tmp;
			}
			zy = 2*zx*zy + cy;
			zx = zxsq - zysq + cx;
			zxsq = zx*zx;
			zysq = zy*zy;
		}
	}
	for (int i=0;i<image_width;i++)
		for (int j=0;j<image_height;j++)
		{
			int val=i*image_height+j;
			cout << pxarr[val] << endl;
			pxarr[val]=(pxarr[val]/maxval)*1000;
			if (pxarr[val]>0 && pxarr[val]<50)
				setColor(i, j, 255, 0, 0);
			if (pxarr[val]>=50 && pxarr[val]<200)
				setColor(i, j, 0, 255, 0);
			if (pxarr[val]>=200 && pxarr[val]<=1000)
				setColor(i, j, 0, 0, 255);
			else
				setColor(i, j, 0, 0, 0);
		}
}
		// iterate over the following several thousand times (at least more times than # of pixels)

		// Create a temporary bucket array (one integer for each pixel
		// Let C be a random point in the complex plane

		//
		// Trace the orbit of C, incrementing the temporary bucket that z falls in for each iteration
		// If Z is in the mandelbrot set, discard the temporary bucket
		// Else, merge the temporary bucket with the outer bucket array


		// Normalize the global bucket array by dividing each value by the maximum value
		// Color each pixel however you wish
		//
		// Parallelizing this function is tricky. It helps to have a list of temporary bucket arrays
		// Which are merged after the computation has finished.

		// Parallelizing is not required, but will save you a lot of time.



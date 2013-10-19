#include "Mandelbrot.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

using namespace std;

#define complexPlaneWidth 2.0
#define complexPlaneLeftEdge -1.5
#define complexPlaneHeight 2.0
#define complexPlaneBottomEdge -1.0

#define cudaErrorCheck(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

namespace{
	__device__ double lerp(double v0, double v1, double t) {
		return v0+(v1-v0)*t;
	}
	__device__ double dist(double count, double x, double y) {
		double zn = sqrt(x + y);
		double mu = logf(logf(zn))/logf(2);
		return count + 1 -mu;
	}
	/*bool checkBulb(double x, double y2) {
		double q = (x - 0.25)*(x - 0.25) - y2;
		return ((4 * q * (q +(x -0.25))<y2) || (16*((x+1)*(x+1)+y2 < 1)));
	}*/
	// Takes in an x pixel coordinate and converts it into the corresponding
	// real coordinate on the complex plane
	__device__ double computeRealFromX(unsigned int x, unsigned int width) {
		// x in the range(0, bitmapwidth)
		double f = (double)x / width;

		// remap x to range(0, complexPlaneWidth)
		double realPart = (f*complexPlaneWidth) + complexPlaneLeftEdge;

		return realPart;
	}

	// Tkaes in a y pixel coordinate and convertes it into the correpsonding
	// imaginary coordinate on the complex plane
	__device__ double computeImaginaryFromY(unsigned int y, unsigned int height) {
		double f = (double)y / height;
		double imaginaryPart = (f*complexPlaneHeight) + complexPlaneBottomEdge;
		return imaginaryPart;
	}

	__device__ void setColor(unsigned char* img, unsigned int x, unsigned int y, unsigned int width, unsigned char red, unsigned char green, unsigned char blue) {
		int offset = (y*width + x)*4;
		img[offset] = red;
		img[offset+1] = green;
		img[offset+2] = blue;
		img[offset+3] = 255;
	}

	//Convert from hsv to rgb values
	__device__ void hsvToRgb(double h, double s, double v, unsigned char rgb[]) {
		double r, g, b;

		int i = (int)(h*6);
		double f = h * 6 - i;
		double p = v * (1 - s);
		double q = v * (1 - f * s);
		double t = v * (1 - (1 - f) * s);

		switch(i % 6){
		case 0: r = v, g = t, b = p; break;
		case 1: r = q, g = v, b = p; break;
		case 2: r = p, g = v, b = t; break;
		case 3: r = p, g = q, b = v; break;
		case 4: r = t, g = p, b = v; break;
		case 5: r = v, g = p, b = q; break;
		}

		rgb[0] = r * 255;
		rgb[1] = g * 255;
		rgb[2] = b * 255;
	}

	__global__ void complex_coord(double* coord, unsigned int width, unsigned int height) {
		int i = blockIdx.x * blockDim.x + threadIdx.x;
		int j = blockIdx.y * blockDim.y + threadIdx.y;

		if( i >= width || j >= height)
			return;

		//Convert from screen coordinates to complex coordinates
		double x0 = computeRealFromX(i, width);
		double y0 = computeImaginaryFromY(j, height);

		int idx = j*width + i;
		coord[idx] = x0;
		coord[idx+1] = y0;
	}
	__global__ void fractal_kernel(double* coord, double* iters, unsigned int width, unsigned int height) {
		//Start z at 0
				int i = blockIdx.x * blockDim.x + threadIdx.x;
		int j = blockIdx.y * blockDim.y + threadIdx.y;

		if( i >= width || j >= height)
			return;
		int idx = j*width+i;

		double x0 = coord[idx];
		double y0 = coord[idx+1];
		double x = 0;
		double y = 0;
		double x2 = 0.0;
		double y2 = 0.0;

		//Iterate until we reach 2 or the max
		int count = 0;
		for(count = 0; count < MAXITER && ((x2 + y2) < 4.0); count++) {
			double xtemp = x2 - y2 + x0;
			y = 2*x*y + y0;
			x = xtemp;
			y2 = y*y;
			x2 = x*x;
		}
		coord[idx] = x2;
		coord[idx+1] = y2;
		iters[idx] = (double)count;
	}
	__global__ void color_fractal(unsigned char* img, double* coord, double* iters, unsigned int width, unsigned int height) {
		int i = blockIdx.x * blockDim.x + threadIdx.x;
		int j = blockIdx.y * blockDim.y + threadIdx.y;
		if( i >= width || j >= height)
			return;
		int idx = j*width+i;

		double iter = iters[idx];
		double x2 = coord[idx];
		double y2 = coord[idx+1];

		unsigned char rgb[3];

		//Color the pixel
		if ((unsigned int)iter == MAXITER) {
			setColor(img, i, j, width, 0, 0, 0);
		}
		else {
			iter = dist(iter, x2, y2);
			//Create the color palatte (0x0, 0xffffff)
			double color = lerp(floor(iter), floor(iter+1), fmod(iter, 1.0));
			double hue = 0.95 * 10 * (color/MAXITER);
			hsvToRgb(hue, 0.6, 1.0, rgb);
			setColor(img, i, j, width, rgb[0], rgb[1], rgb[2]);
		}
	}
}

// Keep track of the complex coordinates and the iteration count in multiple kernels
void Mandelbrot::gen_fractal()
{
	unsigned int dimx = get_width();
	unsigned int dimy = get_height();
	unsigned int size = dimx*dimy;
	unsigned int num_bytes = size*4*sizeof(unsigned char);

	double* d_complex;
	double* d_iters;
	cudaErrorCheck(cudaMalloc((void**)&d_complex, size*sizeof(double)));
	cudaErrorCheck(cudaMalloc((void**)&d_iters, size*sizeof(double)));
	unsigned char* d_a=0;
	cudaErrorCheck(cudaMalloc((void**)&d_a, num_bytes));
	if(0==d_a || 0 == d_complex || 0 == d_iters) {
		cout << "Could not allocate memory" << endl;
		return;
	}

	cudaErrorCheck(cudaMemset(d_a, 0, num_bytes));

	dim3 grid, block;
	block.x = 16;
	grid.x = dimx/block.x;

	block.y = 16;
	grid.y = dimy/block.y;

	complex_coord<<<grid, block>>>(d_complex, dimx, dimy);
	fractal_kernel<<<grid, block>>>(d_complex, d_iters, dimx, dimy);
	color_fractal<<<grid, block>>>(d_a, d_complex, d_iters, dimx, dimy);

	cudaErrorCheck(cudaMemcpy(m_bitmap, d_a, num_bytes,cudaMemcpyDeviceToHost));
	
	cudaFree(d_a);
	cudaFree(d_iters);
	cudaFree(d_complex);
}
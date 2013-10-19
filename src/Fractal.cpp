#include "Fractal.h"
#include <iostream>

using namespace std;

namespace {
    const double complexPlaneWidth = 2;
    const double complexPlaneLeftEdge = -1.5;
    const double complexPlaneHeight = 2.0;
    const double complexPlaneBottomEdge = -1.0;
}

Fractal::Fractal(unsigned int height, unsigned int width)
{
    // Pre-allocate memory for the image data
    m_bitmap = (unsigned char *)malloc(height*width*4*sizeof(unsigned char));
    m_height = height;
    m_width = width;
}

Fractal::Fractal(double x)
{
    unsigned int width = static_cast<int>(3.0*x*100);
    unsigned int height = static_cast<int>(3.0*x*100);
    // Pre-allocate memory for the image data
    m_bitmap = (unsigned char *)malloc(height*width*4*sizeof(unsigned char));
    m_height = height;
    m_width = width;
}

Fractal::~Fractal()
{
    free(m_bitmap);
}

bool Fractal::save_file(string filename) const
{
    unsigned error = lodepng::encode(filename.c_str(), m_bitmap, m_width, m_height);
    if (error)
    {
        std::cout << "lodepng error: " << error << ": " << lodepng_error_text(error) << endl;
        return false;
    }
    return true;
}

unsigned int Fractal::get_width() const
{
    return m_width;
}

unsigned int Fractal::get_height() const
{
    return m_height;
}

// Takes in an x pixel coordinate and converts it into the corresponding
// real coordinate on the complex plane
double Fractal::computeRealFromX(unsigned int x) {
    // x in the range(0, bitmapwidth)
    double f = (double)x / get_width();

    // remap x to range(0, complexPlaneWidth)
    double realPart = (f*complexPlaneWidth) + complexPlaneLeftEdge;

    return realPart;
}

// Tkaes in a y pixel coordinate and convertes it into the correpsonding
// imaginary coordinate on the complex plane
double Fractal::computeImaginaryFromY(unsigned int y) {
    double f = (double)y / get_height();
    double imaginaryPart = (f*complexPlaneHeight) + complexPlaneBottomEdge;
    return imaginaryPart;
}

void Fractal::setColor(unsigned int x, unsigned int y, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
    unsigned char* colorbyte = &(m_bitmap[(y*get_width() + x)*4]);
    *colorbyte = red;
    *(colorbyte+1) = green;
    *(colorbyte+2) = blue;
    *(colorbyte+3) = alpha;
}

//Convert from hsv to rgb values
void Fractal::hsvToRgb(double h, double s, double v, unsigned char rgb[]) {
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

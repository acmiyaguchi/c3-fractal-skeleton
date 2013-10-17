#include "Fractal.h"
#include <iostream>

using namespace std;

namespace {
    const double complexPlaneWidth = 3.5;
    const double complexPlaneLeftEdge = -2.5;
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
    double f = x / get_width();

    // remap x to range(0, complexPlaneWidth)
    double realPart = (f*complexPlaneWidth) + complexPlaneLeftEdge;

    return realPart;
}

// Tkaes in a y pixel coordinate and convertes it into the correpsonding
// imaginary coordinate on the complex plane
double Fractal::computeImaginaryFromY(unsigned y) {
    double f = y / get_height();
    double imaginaryPart = (f*complexPlaneHeight) + complexPlaneBottomEdge;
    return imaginaryPart;
}

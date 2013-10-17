#ifndef FRACTAL_H
#define FRACAL_H

#include<string>
#include<cstdlib>
#include<vector>
#include<iostream>
#include<omp.h>
#include "lodepng.h"

const int MAXITER = 1100;
const int SUBSAMPLING = 20;

class Fractal
{
public:
    Fractal(unsigned int height, unsigned int width);
    Fractal(double x);
    virtual ~Fractal();
    virtual void gen_fractal() = 0;
    bool save_file(std::string filepath) const;
    unsigned int get_width() const;
    unsigned int get_height() const;

    //Helper functions
    double computeRealFromX(unsigned int x);
    double computeImaginaryFromY(unsigned int y);

    void setColor(unsigned int x, unsigned int y, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 0xff);
private:
    unsigned char *m_bitmap;
    unsigned int m_height;
    unsigned int m_width;
};

#endif // FRACTAL_H

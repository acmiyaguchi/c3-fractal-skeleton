#ifndef FRACTAL_H
#define FRACAL_H

#include<string>
#include<cstdlib>
#include<vector>
#include<iostream>
#include<omp.h>
#include "lodepng.h"

const int MAXITER = 1500;
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
    void hsvToRgb(double h, double s, double v, unsigned char rgb[]);
protected:
    unsigned char *m_bitmap;
//    unsigned int m_histogram[MAXITER+1];
    unsigned int m_height;
    unsigned int m_width;
private:
};

#endif // FRACTAL_H

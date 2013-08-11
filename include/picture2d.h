#ifndef __PICTURE2D_H__
#define __PICTURE2D_H__

#include <stdint.h>


// load png files
// converts every file to 32bit RGBA.

class Picture2d
{
public:

    static int num_picture_2d;

    Picture2d();
    ~Picture2d();

    // load a png
    // return false on error
    // can be called only once
    bool load(const char* fname);

    char* pixels();
    void release();

    int  size(); // bytes
    void save_ppm(const char* filename);

    int32_t width;
    int32_t height;
    int32_t bit_depth; // always 32

    // default format: converted to RGBA
    //void setFormat();

    /*void operator = (Picture2d& right)
    {
        width = right.width;
        height = right.height;
        bit_depth = right.bit_depth;
        m_pixels = right.m_pixels;
    }*/
    // always RGBA


private:

    char* m_pixels;
    void m_release_pixels();
};


#endif // __PICTURE2D_H__

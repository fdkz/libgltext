//#define PNG_STATIC
//#define ALL_STATIC
//
// requires: zlib, libpng14 or greater.
//
//#include "stdafx.h"

#include "picture2d.h"

//#include <iostream>
//#include <memory>
#include <stdio.h>
#include <string.h> // strlen, strcpy, ..
#include <stdlib.h> // exit
#include <assert.h>

#include "zlib.h"
#include "png.h"


//#pragma warning( disable : 4996)


// --------------------------------------------------------------------------
// ---- GLOBALS -------------------------------------------------------------
// --------------------------------------------------------------------------


static bool g_picture2d_first_use = true;

struct my_png_info
{
    jmp_buf jmpbuf; // #include <setjmp.h> is already inside png.h
    FILE*   file;
};


// --------------------------------------------------------------------------
// ---- FUNCTIONS -----------------------------------------------------------
// --------------------------------------------------------------------------


void g_png_error_handler(png_structp png_ptr, png_const_charp msg)
{
    printf("libpng: %s\n", msg);

    my_png_info* p = (my_png_info*)png_get_error_ptr(png_ptr);

    if (!p)
    {
        // oh no. crash!
        printf("libpng. serious: no jmpbuf! terminating.\n");
        exit(666);
    }

    longjmp(p->jmpbuf, 1);
}


void g_user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
    my_png_info* p = (my_png_info*)png_get_io_ptr(png_ptr);
    if (length != fread(data, 1, length, p->file))
    {
        printf("libpng: warning! not enough bytes in file.\n");
    }
}


// --------------------------------------------------------------------------
// ---- LIFECYCLE -----------------------------------------------------------
// --------------------------------------------------------------------------


Picture2d::Picture2d()
{
    m_pixels = NULL;
    width = height = bit_depth = 0;
}


Picture2d::~Picture2d()
{
    m_release_pixels();
}


// --------------------------------------------------------------------------
// ---- METHODS -------------------------------------------------------------
// --------------------------------------------------------------------------


// TODO: possible buffer overflow here
void Picture2d::save_ppm(const char* filename)
{
    size_t len = strlen(filename);
    char* fn = new char[len + 5];

    strcpy(fn, filename);
    if (len < 4 || strcmp(fn + len - 4, ".ppm") != 0) strcat(fn, ".ppm");

    FILE* fd = fopen(fn, "wb");
    if (fd == 0) return;

    fprintf(fd, "P6\n%d %d\n255\n", width, height);

    char* buf = new char [width * 3];
    for (int y = 0; y < height; y++)
    {
        // convert current row RGBA->RGB
        char* oldrow = m_pixels + y * 4 * width;
        for (int i = 0; i < width; i++)
        {
            buf[i * 3]     = oldrow[i * 4];
            //buf[i * 3 + 1] = oldrow[i * 4 + 3];
            buf[i * 3 + 1] = oldrow[i * 4 + 1];
            buf[i * 3 + 2] = oldrow[i * 4 + 2];
        }
        fwrite(buf, 1, width * 3, fd);
    }
    delete [] buf;
    delete [] fn;
    fclose(fd);
}


char* Picture2d::pixels()
{    
    return m_pixels;
}


void Picture2d::release()
{
    m_release_pixels();
    width = height = bit_depth = 0;
}


int Picture2d::size()
{
    assert(m_pixels != NULL);
    if (m_pixels) return height * width * (bit_depth >> 3);
    return 0;
}


bool Picture2d::load(const char* fname)
{
    assert(NULL == m_pixels);
    size_t result;

    // http://www.libpng.org/pub/png/book/chapter13.html#png.ch13.div.1
    // Chapter 13. Reading PNG Images
    //
    // http://www.linuxgazette.com/issue90/raghu.html
    // setjmp/longjmp Illustrated . By Raghu J Menon

    // The unfortunate fact is that the ANSI committee responsible for defining
    // the C language and standard C library managed to standardize jmp_buf in
    // such a way that one cannot reliably pass pointers to it, nor can one be
    // certain that its size is constant even on a single system.

    if (g_picture2d_first_use)
    {
        printf("first use of Picture2d:\n");
        printf("  Compiled with libpng '%s'; using libpng '%s'.\n",
                                      PNG_LIBPNG_VER_STRING, png_libpng_ver);
        printf("  Compiled with zlib '%s'; using zlib '%s'.\n",
                                                 ZLIB_VERSION, zlib_version);
        g_picture2d_first_use = false;
    }

    // open the file and check if it is a png file

    FILE* fp;

    fp = fopen(fname, "rb");
    if (fp == NULL)
    {
        printf("png file not found: '%s'\n", fname);
        return false;
    }

    char sig[8];
    result = fread(sig, 1, 8, fp);
    if (result != 8)
    {
        printf("file '%s' is too short. failed to read the first 8 bytes.\n", fname);
        fclose(fp);
        return false;
    }

    if (!png_check_sig((png_bytep)sig, 8))
    {
        printf("file '%s' is probably not a png file.\n", fname);
        fclose(fp);
        return false;
    }

    // try to read the file

    png_structp png_ptr;
    png_infop   info_ptr;
    //unsigned int sig_read = 0;
    //png_uint_32 width, height;
    //int bit_depth, color_type, interlace_type;

    // wtf? 2 errorhandlers at the same time?
    //png_set_error_fn(write_ptr, (png_voidp)inname, pngtest_error,
    //    pngtest_warning);

    my_png_info my_png;
    my_png.file = fp;

    png_ptr = png_create_read_struct(png_libpng_ver, &my_png,
                                     g_png_error_handler, g_png_error_handler);
    if (!png_ptr)
    {
        printf("png_create_read_struct failed.\n");
        fclose(fp);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        printf("png_create_info_struct failed.\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return false;
    }

    // set up our error-hander. every png-error will end up here.
    if (setjmp(my_png.jmpbuf))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return false;
    }

    png_set_read_fn(png_ptr, &my_png, g_user_read_data);


    // now try to actually read the data!


    //image = image.convert('RGBA')
    //imagestring = image.tostring("raw", "RGBA", 0, -1)

    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    // get picture dimensions.

    int color_type;
    int channel_bit_depth;
    png_uint_32 _width;
    png_uint_32 _height;
    png_get_IHDR(png_ptr, info_ptr, &_width,
                 &_height, &channel_bit_depth,
                 &color_type, NULL,
                 NULL, NULL);
    width = _width;
    height = _height;

    // load the picture at last!


    // Extract multiple pixels with bit depths of 1, 2, and 4 from a single
    // byte into separate bytes (useful for paletted and grayscale images).
    //png_set_packing(png_ptr);

    // Change the order of packed pixels to least significant bit first
    // (not useful if you are using png_set_packing).
    //png_set_packswap(png_ptr);

    // Expand paletted colors into true RGB triplets
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    // Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
    if (color_type == PNG_COLOR_TYPE_GRAY && channel_bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    // Expand paletted or RGB images with transparency to full alpha channels
    // so the data will be available as RGBA quartets.
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    // tell libpng to strip 16 bit/color files down to 8 bits/color
    if (channel_bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    // TODO: && color_type != PNG_COLOR_TYPE_GRAY_ALPHA ?
    // Add filler (or alpha) byte (before/after each RGB triplet)
    if (color_type != PNG_COLOR_TYPE_RGB_ALPHA)
        png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);


    //png_set_invert_alpha(png_ptr);


    // update the structs according to the selected transformations.
    // (so png_get_rowbytes() returns something meaningful)
    png_read_update_info(png_ptr, info_ptr);

    // Allocate the memory to hold the image using the fields of info_ptr.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    m_pixels = new char[height * rowbytes];
    unsigned char** row_pointers = new unsigned char*[height];

    // set up our next error-hander
    if (setjmp(my_png.jmpbuf))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        //delete [] m_pixels;
        delete [] m_pixels;
        delete [] row_pointers;
        fclose(fp);
        return false;
    }

    for (int row = 0; row < height; row++)
        row_pointers[row] = (unsigned char*)(m_pixels + row * rowbytes);

    // Now it's time to read the image.  One of these methods is REQUIRED
    // Read the entire image in one go
    png_read_image(png_ptr, row_pointers);

    // read rest of file, and get additional chunks in info_ptr - REQUIRED
    png_read_end(png_ptr, info_ptr);

    bit_depth = 32;

    // the end. if we reach this place, everything is ok..

    if (png_ptr && info_ptr)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        png_ptr = NULL;
        info_ptr = NULL;
    }

    delete [] row_pointers;

    fclose(fp);
    return true;
}


// --------------------------------------------------------------------------
// ---- PRIVATE -------------------------------------------------------------
// --------------------------------------------------------------------------


void Picture2d::m_release_pixels()
{
    delete [] m_pixels;
    m_pixels = NULL;
}

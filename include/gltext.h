#ifndef __GLTEXT_H__
#define __GLTEXT_H__

//
// opengl text renderer
//
//
// reads files made with this tool:
//   http://www.fluidstudios.com/pub/FluidStudios/Tools/Fluid_Studios_Font_Generation_Tool.zip
//
// files i use start like this:
//
//   info: http://www.fluidstudios.com/pub/FluidStudios/Tools/Fluid_Studios_Font_Generation_Tool.zip
//       : ascii, char_x, char_y, byteWidth, byteHeight, xOffset, yOffset, screenWidth, screenHeight
//
//   font_proggy_opti_small.png 512 256
//   0 0 0 0 0 0 0 0 0
//   1 0 0 0 0 0 0 0 0
//   2 0 0 0 0 0 0 0 0
//   ...
// 
// notes:
//
//   sometimes the "space" character (ascii 32) is zero-width. if that's
//   the case, edit the font file manually, modify the screenWidth column.
//

#include <string>


struct GLTexChar;

//
// usage:
//
//   GLText t("font.txt");
//   set_opengl_context();
//   t.init();
//   ...
//   set_pixel_projection() // setup your orthogonal projection matrix. up-left is (0, 0)
//   t.draw..(..);
//
class GLText
{
public:

    // load font
    //
    // font_path_name : data/font_proggy_opti_small.txt
    //                  c:/prog/data/font_proggy_opti_small.txt
    //                  c:\prog/data\font_proggy_opti_small.txt
    GLText(const char* font_path_name);
    virtual ~GLText();

    // call this after activating opengl context. creates the texture.
    void init();

    // return string width in pixels
    float width(const char* text);

    void set_fgcolor(float r, float g, float b, float a);
    void set_bgcolor(float r, float g, float b, float a);
    void set_z(float z);
    // default is true. if false, opengl depth-testing is disabled while rendering.
    // (but still.. z_near/z_far clipping occurs. so use set_z() accordingly.)
    void set_depth_test(bool test);

    // top

    void drawtl(const char* text, float x, float y);
    void drawtr(const char* text, float x, float y);
    void drawtm(const char* text, float x, float y);

    // bottom

    void drawbl(const char* text, float x, float y);
    void drawbr(const char* text, float x, float y);
    void drawbm(const char* text, float x, float y);

    // middle

    void drawml(const char* text, float x, float y);
    void drawmr(const char* text, float x, float y);
    void drawmm(const char* text, float x, float y);

    // baseline

    void drawbll(const char* text, float x, float y);
    void drawblr(const char* text, float x, float y);
    void drawblm(const char* text, float x, float y);

    // if fgcolor or bgcolor or z are None, previous values are used.
    // assumes some kind of pixel-projection..
    // you almost never want to use this method directly.
    void draw(const char* text, float x, float y, int positioning);


    // readonly variables
    float height;
    float ascender;
    float descender;
    float z;
    bool  depth_testing;
    float fgcolor_r, fgcolor_g, fgcolor_b, fgcolor_a;
    float bgcolor_r, bgcolor_g, bgcolor_b, bgcolor_a;

    const char* error; // error str. NULL if no error

    // debug methods for pyrex
    char* font_file_name()    { return (char*)m_font_file_name.c_str(); }
    char* font_file_path()    { return (char*)m_font_file_path.c_str(); }
    char* texture_file_name() { return (char*)m_texture_file_name.c_str(); }


private:

    bool m_initialized;

    std::string m_font_file_path;
    std::string m_font_file_name;
    std::string m_font_folder;
    std::string m_texture_file_name;

    GLTexChar* m_char_list[256];
    int m_char_list_size;

    unsigned int m_texture_id;

    // load everything except the texture. for that, you have to call init()
    // return false on error
    bool m_load_font_data(const char* font_file_path);
    unsigned int m_load_texture(const char* texture_file_path);
    // load and register the texture in opengl. return opengl texture id
    void m_fix_pos(float x, float y, float w, int positioning);
    void m_split_path(const std::string& path, std::string* folder, std::string* filename);
    void m_fix_pos(float x, float y, float w, int positioning, float* x_fix, float* y_fix);
};


struct GLTexChar
{
    float x1, y1, x2, y2; // pos in texture. top-left and bottom-right
    int w, h; // size in texture
    int advance_x;
    int advance_y;
    int ofs_x, ofs_y; // baseline offset. TODO: ofs from where?
};


#endif // __GLTEXT_H__

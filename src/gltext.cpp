#include "gltext.h"

#include <string>
#include <stdio.h>
#include <math.h> // round
#include <assert.h>

//extern "C"
//{
#if defined(_WIN32)
    #include <windows.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#elif defined(__APPLE__)
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else // linux
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif
//}

#include "picture2d.h"

//#include "sys_functions.h"


using namespace std;


// --------------------------------------------------------------------------
// ---- GLOBALS -------------------------------------------------------------
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// ---- HELPERS -------------------------------------------------------------
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// ---- FUNCTIONS -----------------------------------------------------------
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
// ---- LIFECYCLE -----------------------------------------------------------
// --------------------------------------------------------------------------


GLText::GLText(const char* font_file_path)
{
    assert(NULL != font_file_path);

    m_font_file_path = font_file_path;
    m_split_path(m_font_file_path, &m_font_folder, &m_font_file_name);

    m_initialized  = false;

    // split font_path_name to path and filename

    m_char_list_size = 0;

    m_texture_id     = 0;

    height    = 0;
    ascender  = 0;
    descender = 0;

    z         = 0.;
    depth_testing = true;

    fgcolor_r = 1.;
    fgcolor_g = 1.;
    fgcolor_b = 1.;
    fgcolor_a = 1.;

    bgcolor_r = 1.;
    bgcolor_g = 1.;
    bgcolor_b = 1.;
    bgcolor_a = 0.;

    this->error = NULL;

    m_load_font_data(font_file_path);
}


GLText::~GLText()
{
    for (int i = 0; i < m_char_list_size; i++) delete m_char_list[i];
}


// --------------------------------------------------------------------------
// ---- METHODS -------------------------------------------------------------
// --------------------------------------------------------------------------


void GLText::init()
{
    assert(!m_initialized);
    assert(!this->error);
    string s = m_font_folder + "/" + m_texture_file_name;
    m_texture_id = m_load_texture(s.c_str());
    if (!this->error) m_initialized = true;
}


float GLText::width(const char* text)
{
    assert(m_initialized);

    float str_width = 0;
    while(*text)
    {
        assert((unsigned char)(*text) < m_char_list_size);
        //printf("%i\n", (unsigned char)(*text));
        str_width += m_char_list[(unsigned char)(*text)]->advance_x;
        text++;
    }
    return str_width;
}


void GLText::set_fgcolor(float r, float g, float b, float a)
{
    assert(m_initialized);
    fgcolor_r = r;
    fgcolor_g = g;
    fgcolor_b = b;
    fgcolor_a = a;
}


void GLText::set_bgcolor(float r, float g, float b, float a)
{
    assert(m_initialized);
    bgcolor_r = r;
    bgcolor_g = g;
    bgcolor_b = b;
    bgcolor_a = a;
}


void GLText::set_z(float _z)
{
    assert(m_initialized);
    z = _z;
}


void GLText::set_depth_test(bool test)
{
    depth_testing = test;
}


// top

void GLText::drawtl(const char* text, float x, float y)
{   draw(text, x, y, 1+16); }
void GLText::drawtr(const char* text, float x, float y)
{   draw(text, x, y, 2+16); }
void GLText::drawtm(const char* text, float x, float y)
{   draw(text, x, y, 4+16); }

// bottom

void GLText::drawbl(const char* text, float x, float y)
{   draw(text, x, y, 1+32); }
void GLText::drawbr(const char* text, float x, float y)
{   draw(text, x, y, 2+32); }
void GLText::drawbm(const char* text, float x, float y)
{   draw(text, x, y, 4+32); }

// middle

void GLText::drawml(const char* text, float x, float y)
{   draw(text, x, y, 1+64); }
void GLText::drawmr(const char* text, float x, float y)
{   draw(text, x, y, 2+64); }
void GLText::drawmm(const char* text, float x, float y)
{   draw(text, x, y, 4+64); }

// baseline

void GLText::drawbll(const char* text, float x, float y)
{   draw(text, x, y, 1+8); }
void GLText::drawblr(const char* text, float x, float y)
{   draw(text, x, y, 3+8); }
void GLText::drawblm(const char* text, float x, float y)
{   draw(text, x, y, 4+8); }


void GLText::draw(const char* text, float x, float y, int positioning)
{
    assert(m_initialized);

    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT);

    //glDisable(GL_ALPHA_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glShadeModel(GL_FLAT);
    glDepthFunc(GL_LEQUAL);

    if (!this->depth_testing) glDisable(GL_DEPTH_TEST);

    // calc string width
    float str_width = this->width(text);
    m_fix_pos(x, y, str_width, positioning, &x, &y);

    if (str_width > 0)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // draw the background

        if (bgcolor_a != 0.)
        {
            float up_left_x = x;
            float up_left_y = y - ascender;
            float down_right_x = x + str_width + 1;
            float down_right_y = y + descender; // TODO: swap asc-desc by fontheight?

            glDisable(GL_TEXTURE_2D);
            glColor4f(bgcolor_r, bgcolor_g, bgcolor_b, bgcolor_a);

            glBegin(GL_QUADS);
            glVertex3f(up_left_x,    up_left_y,    z);
            glVertex3f(down_right_x, up_left_y,    z);
            glVertex3f(down_right_x, down_right_y, z);
            glVertex3f(up_left_x,    down_right_y, z);
            glEnd();
        }

        // draw the text

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_texture_id);

        glColor4f(fgcolor_r, fgcolor_g, fgcolor_b, fgcolor_a);

        glBegin(GL_QUADS);

        int i = 0;
        while (text[i])
        {
            GLTexChar* ch = m_char_list[ (unsigned char)text[i] ];

            // up-left
            glTexCoord2f(ch->x1, ch->y1);
            glVertex3f(x + ch->ofs_x, y + ch->ofs_y, z);
            // up-right
            glTexCoord2f(ch->x2, ch->y1);
            glVertex3f(x + ch->ofs_x + ch->w, y + ch->ofs_y, z);
            // bottom-right
            glTexCoord2f(ch->x2, ch->y2);
            glVertex3f(x + ch->ofs_x + ch->w, y + ch->ofs_y + ch->h, z);
            // bottom-left
            glTexCoord2f(ch->x1, ch->y2);
            glVertex3f(x + ch->ofs_x, y + ch->ofs_y + ch->h, z);

            x += ch->advance_x;

            i++;
        }

        glEnd();
    }

    glPopAttrib();
}


// --------------------------------------------------------------------------
// ---- PRIVATE -------------------------------------------------------------
// --------------------------------------------------------------------------


void GLText::m_split_path(const string& path, string* folder, string* filename)
{
    size_t found = path.find_last_of("/\\");
    *folder   = path.substr(0, found);
    *filename = path.substr(found + 1);
    //m_font_file_name = new char[strlen(font_file_name) + 1];
    //strcpy(m_font_file_name, font_file_name);
}


// font_file_path : data/font_proggy_opti_small.txt
//                  c:/prog/data/font_proggy_opti_small.txt
//                  c:\prog/data\font_proggy_opti_small.txt
//
// return false on error
//
bool GLText::m_load_font_data(const char* font_file_path)
{
    assert(0 == m_char_list_size);
    assert(!m_initialized);

    FILE* f = fopen(font_file_path, "rb");
    if (!f)
    {
        this->error = "font file opening failed (not found?)";
        return false;
    }

    char line[1000];
    char tempstr[1000];
    int texture_width, texture_height;

    // read three lines. if lines are > 1000 characters, will not read three lines :(

    fgets(line, sizeof(line), f);
    fgets(line, sizeof(line), f);
    fgets(line, sizeof(line), f);

    fgets(line, sizeof(line), f);
    sscanf(line, "%1000s %i %i", tempstr, &texture_width, &texture_height);
    m_texture_file_name = tempstr;

    int i = 0;
    while (i < 256)
    {
        GLTexChar* c = new GLTexChar();
        fgets(line, sizeof(line), f); // !feof(f)
        int ascii;

        //printf("'%s'", line);

        // ascii, char_x, char_y, byteWidth, byteHeight, xOffset, yOffset, screenWidth, screenHeight
        int count = sscanf(line, "%i %f %f %i %i %i %i %i %i", &ascii, &c->x1, &c->y1, &c->w, &c->h, &c->ofs_x, &c->ofs_y, &c->advance_x, &c->advance_y);
        if (i != ascii || count != 9)
        {
            this->error = "font loading error";
            return false;
        }
        // texture coordinates
        c->x1 /= texture_width;
        c->y1 /= texture_height;
        c->x2 = c->x1 + (float)c->w / texture_width;
        c->y2 = c->y1 + (float)c->h / texture_height;

        m_char_list[m_char_list_size] = c;
        m_char_list_size++;

        //if (c->h > this->height) this->height = c->h; // TODO: shouldn't ofs_y also be used here?

        if (c->x1 > 1. || c->x1 < 0. ||
            c->y1 > 1. || c->y1 < 0. ||
            c->x2 > 1. || c->x2 < 0. ||
            c->y2 > 1. || c->y2 < 0.)
        {
            this->error = "char out of texture bounds";
            return false;
        }
        i++;
    }

    fgets(line, sizeof(line), f);
    int count = sscanf(line, "%f %f %f", &this->ascender, &this->descender, &this->height);
    if (count != 3 || feof(f))
    {
        error = "font loading error. last line?";
        return false;
    }

    return true;
}


void GLText::m_fix_pos(float x, float y, float w, int positioning, float* x_fix, float* y_fix)
{
    // positioning
    // first 3 bits - left-right-middle
    // next  4 bits - baseline-top-bottom-middle

    if (positioning)
    {
        if      (positioning & 1) { }           // left
        else if (positioning & 2) x -= w;       // right
        else if (positioning & 4) x -= w / 2.f; // middle

        if      (positioning & 8) { }                     // baseline
        else if (positioning & 16) y += this->ascender;   // top
        else if (positioning & 32) y -= this->descender;  // bottom
        else if (positioning & 64) y += this->height / 2.f - this->descender; // middle
    }

    *x_fix = floor(x + 0.5f); // round(x); // round missing in visual studio express 2008
    *y_fix = floor(y + 0.5f); // round(y);
}


unsigned int GLText::m_load_texture(const char* texture_file_path)
{
    // load and register the texture in opengl. return opengl texture id

    Picture2d image;
    if (!image.load(texture_file_path))
    {
        error = "texture file loading failed";
        return 0;
    }

    GLuint tex_id;
    glGenTextures(1, &tex_id);

    glBindTexture(GL_TEXTURE_2D, tex_id);

    // change state of the selected texture object
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels());
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return tex_id;
}

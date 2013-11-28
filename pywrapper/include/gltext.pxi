
cdef extern from "sys_functions.h":

    void cpp_setloggers "setloggers" (int stdout_fileno, int stderr_fileno)


cdef extern from "gltext.h":

    cdef cppclass cpp_GLText "GLText":

        cpp_GLText(char* font_path_name)

        void  init()
        float width(char* text)


        void set_fgcolor    (float r, float g, float b, float a)
        void set_bgcolor    (float r, float g, float b, float a)
        void set_z          (float z)
        void set_depth_test (int test)

        void drawtl  (char* text, float x, float y)
        void drawtr  (char* text, float x, float y)
        void drawtm  (char* text, float x, float y)

        void drawbl  (char* text, float x, float y)
        void drawbr  (char* text, float x, float y)
        void drawbm  (char* text, float x, float y)

        void drawml  (char* text, float x, float y)
        void drawmr  (char* text, float x, float y)
        void drawmm  (char* text, float x, float y)

        void drawbll (char* text, float x, float y)
        void drawblr (char* text, float x, float y)
        void drawblm (char* text, float x, float y)


        float height
        float ascender
        float descender

        float z
        int   depth_testing

        float fgcolor_r, fgcolor_g, fgcolor_b, fgcolor_a
        float bgcolor_r, bgcolor_g, bgcolor_b, bgcolor_a


        char* font_file_name    ()
        char* font_file_path    ()
        char* texture_file_name ()
        char* error


cdef class GLText:

    cdef cpp_GLText* thisptr

    def __cinit__(self, char* font_path_name):
        self.thisptr = new cpp_GLText(font_path_name)
        if self.thisptr.error:
            raise Exception("error loading font '%s': %s" % (self.thisptr.font_file_path(), self.thisptr.error))

    def __dealloc__(self):
        del self.thisptr


    def init(self):
        self.thisptr.init()
        if self.thisptr.error:
            raise Exception("error loading font '%s' texture '%s': %s" % (self.thisptr.font_file_path(), self.thisptr.texture_file_name(), self.thisptr.error))

    def width(self, char* text):
        """ return string width in pixels """
        return self.thisptr.width(text)


    property height:
        def __get__(self): return self.thisptr.height
    property ascender:
        def __get__(self): return self.thisptr.ascender
    property descender:
        def __get__(self): return self.thisptr.descender
    property z:
        def __get__(self): return self.thisptr.z
    property depth_testing:
        def __get__(self):
            if self.thisptr_depth_testing: return True
            return False
    property fgcolor:
        def __get__(self): return (self.thisptr.fgcolor_r, self.thisptr.fgcolor_g, self.thisptr.fgcolor_b, self.thisptr.fgcolor_a)
        def __set__(self, val): self.thisptr.fgcolor_r, self.thisptr.fgcolor_g, self.thisptr.fgcolor_b, self.thisptr.fgcolor_a = val
    property bgcolor:
        def __get__(self): return (self.thisptr.bgcolor_r, self.thisptr.bgcolor_g, self.thisptr.bgcolor_b, self.thisptr.bgcolor_a)
        def __set__(self, val): self.thisptr.bgcolor_r, self.thisptr.bgcolor_g, self.thisptr.bgcolor_b, self.thisptr.bgcolor_a = val


    def drawtl(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawtl(text, x, y)
    def drawtr(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawtr(text, x, y)
    def drawtm(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawtm(text, x, y)

    # bottom

    def drawbl(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawbl(text, x, y)
    def drawbr(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawbr(text, x, y)
    def drawbm(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawbm(text, x, y)

    # middle

    def drawml(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawml(text, x, y)
    def drawmr(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawmr(text, x, y)
    def drawmm(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawmm(text, x, y)

    # baseline

    def drawbll(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawbll(text, x, y)
    def drawblr(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawblr(text, x, y)
    def drawblm(self, char* text, float x, float y, fgcolor=None, bgcolor=None, z=None):
        self._set_colorz(fgcolor, bgcolor, z)
        self.thisptr.drawblm(text, x, y)

    def _set_colorz(self, fgcolor, bgcolor, z):
        if fgcolor: self.thisptr.set_fgcolor(fgcolor[0], fgcolor[1], fgcolor[2], fgcolor[3])
        if bgcolor: self.thisptr.set_bgcolor(bgcolor[0], bgcolor[1], bgcolor[2], bgcolor[3])
        if z is False: self.thisptr.set_depth_test(0)
        elif z is not None:
            self.thisptr.set_z(z)
            self.thisptr.set_depth_test(1)


def setloggers(int stdout_fileno, int stderr_fileno):
#outfile.fileno(), outfile.fileno()):
    cpp_setloggers(stdout_fileno, stderr_fileno)

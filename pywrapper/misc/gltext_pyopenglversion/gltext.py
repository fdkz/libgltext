import os.path

from OpenGL.GL import *
from PIL import Image


class GLText:
    """"
    opengl text renderer

    but you still have to set up the orthogonal projection matrix yourself.

    t = gltext.GLText("font.txt")
    w = gltext.width("asd")
    set_opengl_context()
    t.init()
    ...
    t.draw..()

    """

    def __init__(self, font_file_name = "font_proggy_opti_small.txt"):
        """
        load font

        sometimes the "space" character (ascii 32) is zero-width. if that's
        the case, edit the font file manually, modify the screenWidth column.
        """

        self.initialized       = False

        self.font_file_name    = font_file_name
        self.texture_file_name = ""
        self.char_list    = []
        self.texture_size = [0, 0]
        self.texture_id   = 0

        self.height    = 0
        self.ascender  = 0
        self.descender = 0

        self.z       = False
        self.fgcolor = (1.0, 1.0, 1.0, 1.0)
        self.bgcolor = (1.0, 1.0, 1.0, 0.0)

        self._load_font_data(self.font_file_name)


    def init(self):

        assert not self.initialized
        self.texture_id  = self._load_texture(os.path.join(os.path.dirname(self.font_file_name), self.texture_file_name))
        self.initialized = True


    def width(self, text):
        """ return string width in pixels """

        str_width = 0
        for c in text:
            str_width += self.char_list[ord(c)].advance[0]
        return str_width


    # top

    def drawtl(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 1+16)

    def drawtr(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 2+16)

    def drawtm(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 4+16)

    # bottom

    def drawbl(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 1+32)

    def drawbr(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 2+32)

    def drawbm(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 4+32)

    # middle

    def drawml(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 1+64)

    def drawmr(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 2+64)

    def drawmm(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 4+64)

    # baseline

    def drawbll(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 1+8)

    def drawblr(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 2+8)

    def drawblm(self, text, x, y, fgcolor=None, bgcolor=None, z=None):
        self.draw(text, x, y, fgcolor, bgcolor, z, 4+8)


    def draw(self, text, x, y, fgcolor=None, bgcolor=None, z=None, positioning=None):
        """
        if fgcolor or bgcolor or z are None, previous values are used.
        if z is False, opengl depth-testing is disabled for text rendering. (but still.. z_near/z_far clipping occurs)
        assumes some kind of pixel-projection..
        """

        assert self.initialized

        if z or z is False: self.z       = z
        if fgcolor:         self.fgcolor = fgcolor
        if bgcolor:         self.bgcolor = bgcolor

        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT)

        #glDisable(GL_ALPHA_TEST)
        glDisable(GL_LIGHTING)
        glDisable(GL_FOG)
        glShadeModel(GL_FLAT)
        glDepthFunc(GL_LEQUAL)

        if self.z is False:
            glDisable(GL_DEPTH_TEST)
            z = 0.
        else:
            #glEnable(GL_DEPTH_TEST)
            z = self.z

        # calc string width
        # TODO: apply. reduce. sum. map.. anything that can speed things up!!
        str_width = self.width(text)
        x, y = self._fix_pos(x, y, str_width, positioning)

        if str_width > 0:

            glEnable(GL_BLEND)
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

            # draw the background

            if self.bgcolor[3] != 0.:

                char       = self.char_list[ ord(text[0]) ]
                up_left    = (x, y - self.ascender)
                down_right = (x + str_width + 1, y + self.descender) # TODO: swap asc-desc by fontheight?

                glDisable(GL_TEXTURE_2D)
                glColor4f(self.bgcolor[0], self.bgcolor[1], self.bgcolor[2], self.bgcolor[3])

                # TODO: think this through. fractional coordinates.
                glBegin(GL_QUADS)
                glVertex3f(up_left[0],    up_left[1],    z)
                glVertex3f(down_right[0], up_left[1],    z)
                glVertex3f(down_right[0], down_right[1], z)
                glVertex3f(up_left[0],    down_right[1], z)
                glEnd()

            # draw the text

            glEnable(GL_TEXTURE_2D)
            glBindTexture(GL_TEXTURE_2D, self.texture_id)

            glColor4f(self.fgcolor[0], self.fgcolor[1], self.fgcolor[2], self.fgcolor[3])

            glBegin(GL_QUADS)

            for c in text:
                char = self.char_list[ord(c)]

                # up-left
                glTexCoord2f(char.pos1[0], char.pos1[1])
                glVertex3f(x + char.ofs[0], y + char.ofs[1], z)
                # up-right
                glTexCoord2f(char.pos2[0], char.pos1[1])
                glVertex3f(x + char.ofs[0] + char.size[0], y + char.ofs[1], z)
                # bottom-right
                glTexCoord2f(char.pos2[0], char.pos2[1])
                glVertex3f(x + char.ofs[0] + char.size[0], y + char.ofs[1] + char.size[1], z)
                # bottom-left
                glTexCoord2f(char.pos1[0], char.pos2[1])
                glVertex3f(x + char.ofs[0], y + char.ofs[1] + char.size[1], z)

                x += char.advance[0]

            glEnd()

        glPopAttrib()


    def _load_font_data(self, font_file_name):
        """
        load everything except the texture. for that, you have to call init()
        """

        f = file(font_file_name)

        f.readline()
        f.readline()
        f.readline()

        line = f.readline().split(" ")
        self.texture_file_name = line[0]
        self.texture_size      = ( int(line[1]), int(line[2]) )
        for i in range(256):
            c = GLTexChar()
            line = f.readline().split(" ")
            # ascii, char_x, char_y, byteWidth, byteHeight, xOffset, yOffset, screenWidth, screenHeight
            if i != int(line[0]): raise ValueError, "font loading error"
            tsx, tsy = self.texture_size
            # texture coordinates
            c.pos1 = ( float(line[1]) / tsx, \
                       float(line[2]) / tsy)
            c.pos2 = (c.pos1[0] + float(line[3]) / tsx, \
                      c.pos1[1] + float(line[4]) / tsy)
            c.size    = (float(line[3]), float(line[4]))
            c.ofs     = (float(line[5]), float(line[6]))
            c.advance = (float(line[7]), float(line[8]))

            self.char_list.append(c)

            if c.size[1] > self.height: self.height = c.size[1]

            if c.pos1[0] > 1 or c.pos1[0] < 0 or \
               c.pos1[1] > 1 or c.pos1[1] < 0 or \
               c.pos2[0] > 1 or c.pos2[0] < 0 or \
               c.pos2[1] > 1 or c.pos2[1] < 0: raise ValueError, "char out of texture bounds"

        line = f.readline().split(" ")
        self.ascender, self.descender, self.height = int(line[0]), int(line[1]), int(line[2])


    def _load_texture(self, texture_file_name):
        """ load and register the texture in opengl. return opengl texture id """

        image = Image.open(texture_file_name)
        ix = image.size[0]
        iy = image.size[1]

        #image = image.convert('RGBX')
        #imagestring = image.tostring("raw", "RGBX", 0, -1) # -1 - swap y
        image = image.convert('RGBA')
        imagestring = image.tostring("raw", "RGBA", 0)

        tex_id = glGenTextures(1)

        glBindTexture(GL_TEXTURE_2D, tex_id)

        # change state of the selected texture object
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ix, iy, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagestring)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)

        return tex_id


    def _fix_pos(self, x, y, w, positioning):

        # positioning
        # first 3 bits - left-right-middle
        # next  4 bits - baseline-top-bottom-middle

        if positioning is not None:

            if   positioning & 1: pass        # left
            elif positioning & 2: x -= w      # right
            elif positioning & 4: x -= w / 2. # middle

            if   positioning & 8:  pass                  # baseline
            elif positioning & 16: y += self.ascender    # top
            elif positioning & 32: y -= self.descender   # bottom
            elif positioning & 64: y += self.height / 2. - self.descender # middle

        return round(x), round(y)


class GLTexChar:
    pass
    #
    # pos1, pos2
    # size
    # advance
    # ofs
    #


def init(font_path = ""):
    """
    some convenience-routines. just init some default fonts.
    but make sure that opengl-context is active while calling this.
    """

    global small
    small = GLText(os.path.join(font_path, "font_proggy_opti_small.txt"))
    small.init()

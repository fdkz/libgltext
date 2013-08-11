"""

generates new font files (txt and png) where every symbol has at
least 1px gap between itself and its neighbor.

usage:

    fontgapper.py font_proggy_opti_small.txt

"""


import sys
from PIL import Image


class GLText:

    def __init__(self):

        self.image_in  = None
        self.image_out = None
        self.x, self.y = 1, 1
        self.line_h = 0
        self.w, self.h = 256, 256

    def copy_char(self, x, y, w, h):

        i = self.image_in.crop((x, y, w + x, h + y))
        #print x, y, w, h, i.size, self.image_in.size
        iw, ih = self.image_out.size
        if self.x + w + 1 > iw:
            self.x = 1
            self.y += self.line_h + 1
            self.line_h = 0
        if self.y + h + 1 > ih: raise RuntimeError, "ERROR! image_out too small"

        self.image_out.paste(i, (self.x, self.y, self.x + w, self.y + h))

        self.line_h = max(self.line_h, h)
        xx = self.x
        if w > 0: self.x += w + 1
        return xx, self.y

    def open_images(self, in_filename, w, h):

        self.image_in = i = Image.open(in_filename)
        print "info about '%s':" % in_filename, i.mode, i.size, i.format, i.info, i.getpixel((256,128))
        assert w == i.size[0] and h == i.size[1]

        self.image_out = Image.new(i.mode, (self.w, self.h), (0,0,0,0))
        #self.image_out.show()


    def generate_new_font(self, font_file_name, prepend="gap_"):
        """
        generates new font files (txt and png) where every symbol has at
        least 1px gap between itself and its neighbor.
        """

        f  = open(font_file_name)
        out_font_filename = prepend + font_file_name
        fo = open(out_font_filename, "wb")

        fo.write(f.readline())
        fo.write(f.readline())
        fo.write(f.readline())

        line = f.readline().split(" ")
        out_texture_filename = prepend + line[0]
        fo.write("%s %s %s\n" % (out_texture_filename, self.w, self.h))
        texture_filename = line[0]
        texture_size      = ( int(line[1]), int(line[2]) )
        self.open_images(texture_filename, texture_size[0], texture_size[1])
        for i in range(256):
            line = f.readline().split(" ")
            # ascii, char_x, char_y, byteWidth, byteHeight, xOffset, yOffset, screenWidth, screenHeight
            if i != int(line[0]): raise ValueError, "font loading error"
            x, y = (int(line[1]), int(line[2]))
            w, h = (int(line[3]), int(line[4]))

            newpos = self.copy_char(x, y, w, h)
            line[1] = str(newpos[0])
            line[2] = str(newpos[1])
            fo.write(" ".join(line))

        line = f.readline()
        fo.write(line)
        line = line.split(" ")

        self.image_out.save(out_texture_filename)
        print "wrote '%s' and '%s'" % (out_font_filename, out_texture_filename)


t = GLText()
t.generate_new_font(sys.argv[1])


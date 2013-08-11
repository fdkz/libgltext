
import pygame
from pygame.locals import *
from OpenGL.GL import *

import fps_counter

import gltext.gltext as gltext
#import gltext
#import gltext_wrapper as gltext

#
# input: g_screen
#        g_text
#
def frame():

    glClearColor(0.,0.,0.,1.)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    w, h = g_screen.get_width(), g_screen.get_height()

    g_text.drawtl("top-left",     0, 0, fgcolor = (1.0, 1.0, 1.0, 1.0), bgcolor = (0., 0., 0., 0.), z = 1000.)

    for i in xrange(1000):
        g_text.drawtl("bottom-left", i / 5., i / 4.)

    g_text.drawbl("bottom-left",  0, h)
    g_text.drawtr("top-right",    w, 0)
    g_text.drawbr("bottom-right", w, h)

    g_text.drawmm("middle", w / 2, h / 2, bgcolor = (0.3, 0.4, 0.5, 1.))

    global g_fps
    g_text.drawmm("fps: %i" % (g_fps), w / 2, h / 2 + g_text.height, fgcolor = (1.0, 0.7, 0.7, 1.0), bgcolor = (0., 0., 0., 0.))


#
# input: g_screen
#
def init_gl():

    glDisable(GL_TEXTURE_2D)
    glDisable(GL_DEPTH_TEST)
    glDisable(GL_FOG)
    glDisable(GL_DITHER)
    glDisable(GL_LIGHTING)
    glShadeModel(GL_FLAT)
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    glEnable(GL_LINE_SMOOTH)
    glDisable(GL_LINE_STIPPLE)

    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    z_near, z_far = 101., 100000.
    w, h = g_screen.get_width(), g_screen.get_height()
    glViewport(0, 0, w, h)
    glOrtho(0., w, h, 0., z_near, z_far)

    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    glScale(1., 1., -1.)


def main():

    global g_fps

    clock   = pygame.time.Clock()
    #prev_time = time.time()
    fpscounter = fps_counter.FpsCounter()
    #fpsinterval = 0

    counter = 0
    do_quit = False

    init_gl()

    while 1:

        for event in pygame.event.get():

            if event.type == QUIT: do_quit = True

            if event.type == KEYDOWN:
                if event.key == K_ESCAPE: do_quit = True

        if do_quit: break

        #time.sleep(1 / 100.)
        frame()
        pygame.display.flip()

        dt = clock.tick()
        fpscounter.tick(dt/1000.)
        g_fps = fpscounter.fps

        #counter += 1
        #if counter >= 20:
        #    counter = 0
        #    g_fps = clock.get_fps()


print "-" * 78
pygame.init()

g_screen = pygame.display.set_mode((640, 480), DOUBLEBUF | OPENGL)
pygame.display.set_caption('O_o')
pygame.mouse.set_visible(1)

g_fps = 0
g_text = gltext.GLText("../../data/font_proggy_opti_small.txt")
g_text.init()

main()


pygame.quit()

print "-- quit ok", "-" * 67

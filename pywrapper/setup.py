from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

import sys


libraries = []
extra_compile_args = []
extra_link_args = []

if sys.platform == "darwin":
    # negate the -g debugging flag that got always added in macosx by default
    extra_compile_args = ["-g0"]
    #include_dirs = ["/usr/local/include"]
elif sys.platform == "win32":
    libraries = ["OpenGL32"]
elif sys.platform == "linux2":
    pass
else:
    raise RuntimeError("platform %s not supported" % (sys.platform))


ext_gltext = Extension(
    "gltext",
    language="c++",
    #pyrex_cplus=1,
    sources=["gltext.pyx",
             "../src/gltext.cpp",
             "../src/stb_image.c"],
    include_dirs=["../include/"],# + include_dirs,
    library_dirs=[],
    libraries=libraries,
    #define_macros=[("WIN32", "1")],
    extra_link_args=extra_link_args,
    extra_compile_args=extra_compile_args,
    #depends=[],
    )


setup(
    name         = "gltext",
    version      = "0.2.1",
    license      = "MIT",
    description  = "python wrapper for a simple opengl text renderer",
    long_description  = """nah""",
    keywords     = "",
    author       = "",
    author_email = "",
    url          = "",
    classifiers  = [],
    ext_modules  = [ext_gltext],
    cmdclass={
        'build_ext': build_ext,
        }
    )


# the setup.py "clean" command is not working completely for cython. we'll do it ourselves.

if "clean" in sys.argv:
    import os
    import shutil
    for p in ["gltext.cpp"]:
        print "deleting", p
        try:    os.remove(p)
        except: pass
    shutil.rmtree("build", True)


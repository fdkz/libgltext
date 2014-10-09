libgltext
=========

Simple opengl text renderer.
There's no makefile except only for the python wrapper.


Building python wrappers:

    cd pywrapper
    python setup.py build

    macosx Yosemite: works without problems.
    macosx Mountain Lion: for build errors workaround look further down.

    Windows 8.1: tested with Visual C++ 2012 Express, Python 2.7.3 64 bit
        run cmd.exe
        set VS90COMNTOOLS=%VS110COMNTOOLS%
        call "C:\devtools\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x86_amd64
        python setup.py build


Prerequisites:

    pip install cython

macosx Mountain Lion build errors workaround:

  To prevent errors like this:

    /usr/libexec/gcc/powerpc-apple-darwin10/4.0.1/as: assembler (/usr/bin/../libexec/gcc/darwin/ppc/as or /usr/bin/../local/libexec/gcc/darwin/ppc/as) for architecture ppc not installed
    Installed assemblers are:
    /usr/bin/../libexec/gcc/darwin/x86_64/as for architecture x86_64
    /usr/bin/../libexec/gcc/darwin/i386/as for architecture i386

  disable ppc support:

    export ARCHFLAGS="-arch i386 -arch x86_64"; python setup.py build


Random notes:

    python setup.py build > log.txt 2>&1
    python setup.py build -c mingw32 > log.txt 2>&1


Third-party libraries:

    Uses [stb_image](http://nothings.org) for image loading.
    Uses the [Fluid Studios](http://www.paulnettle.com/) FontGen tool to generate font textures (tools/Fluid_Studios_Font_Generation_Tool.zip).

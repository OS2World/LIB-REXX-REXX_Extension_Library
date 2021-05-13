# Makefile for IBM C Compiler 3.x and NMAKE32.
CC     = icc
LINK   = ilink
CFLAGS = /Ss /Q /Gm /Ge- /Wall+ext-par-ppt-trd-uni-
LFLAGS = /NOLOGO /MAP /EXEPACK:2
NAME   = rexxlib
LIBS   = rexx.lib rxstring.lib

BL_DESC   = "REXX Extension Library ¸ 2001-2004 D.J. van Enckevort"
BL_AUTHOR = "D.J. van Enckevort"

# Set environment variable USE_RUNTIME (=anything) to use the IBM C runtime
# instead of linking statically
!ifdef USE_RUNTIME
    CFLAGS = $(CFLAGS) /Gd
!endif

# Set environment variable DEBUG (=anything) to build with debugging symbols
!ifdef DEBUG
    CFLAGS = $(CFLAGS) /Ti /Tm
    LFLAGS = $(LFLAGS) /DEBUG
!endif

$(NAME).dll : rexxlib.obj rexxlib.h
                @makedesc -N$(BL_AUTHOR) -D$(BL_DESC) -V"^#define=STR_VERSION,rexxlib.h" rexxlib.def
                $(LINK) $(LFLAGS) $< rexxlib.def $(LIBS) /O:$@
!ifdef USE_RUNTIME
                @dllrname.exe $@ CPPOM30=OS2OM30 /Q /R
!endif

rexxlib.obj : rexxlib.c rexxlib.h rexxlib.def
                $(CC) $(CFLAGS) /C /Ge- $<

clean       :
                if exist $(NAME).dll del $(NAME).dll
                if exist rexxlib.obj del rexxlib.obj
                if exist $(NAME).map del $(NAME).map



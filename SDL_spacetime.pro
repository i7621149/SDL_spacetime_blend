# This specifies the exe name
TARGET=SDL_spacetime
# where to put the .o files
OBJECTS_DIR=obj

# as I want to support 4.8 and 5 this will set a flag for some of the mac stuff
# mainly in the types.h file for the setMacVisual which is native in Qt5
isEqual(QT_MAJOR_VERSION, 5) {
        cache()
        DEFINES +=QT5BUILD
}
!win32:{
# this demo uses SDL so add the paths using the sdl2-config tool
QMAKE_CXXFLAGS+=$$system(sdl2-config  --cflags)
message(output from sdl2-config --cflags added to CXXFLAGS= $$QMAKE_CXXFLAGS)

LIBS+=$$system(sdl2-config  --libs)
message(output from sdl2-config --libs added to LIB=$$LIBS)
}

win32 :{
    message(Make sure that SDL2 is installed in C:\SDL2 and the libs are built)
    INCLUDEPATH+=C:\SDL2\include
    LIBS+=-LC:\SDL2\VisualC\SDL\x64\Debug -lSDL2
    LIBS+=-LC:\SDL2\VisualC\SDLmain\x64\Debug -lSDL2main

}

LIBS+= -lSDL2_image

# where to put moc auto generated file
MOC_DIR=moc
# on a mac we don't create a .app bundle file ( for ease of multiplatform use)
CONFIG-=app_bundle

# Include all .cpp files in the project src directory
SOURCES+= $$PWD/src/main.cpp \
          $$PWD/src/Scene.cpp \
          $$PWD/src/Shader.cpp \
          $$PWD/src/Shapes.cpp

# same for the .h files
HEADERS+= $$PWD/include/Scene.hpp \
          $$PWD/include/Shader.hpp \
          $$PWD/include/Shapes.hpp

# and add the include dir into the search path for Qt and make
INCLUDEPATH +=./include
# where our exe is going to live (root of project)
DESTDIR=./
# were are going to default to a console app
CONFIG += console
# note each command you add needs a ; as it will be run as a single line
# first check if we are shadow building or not easiest way is to check out against current
!equals(PWD, $${OUT_PWD}){
        copydata.commands = echo "creating destination dirs" ;
        # now make a dir
        copydata.commands += mkdir -p $$OUT_PWD/shaders ;
        copydata.commands += echo "copying files" ;
        # then copy the files
        copydata.commands += $(COPY_DIR) $$PWD/shaders/* $$OUT_PWD/shaders/ ;
        # now make sure the first target is built before copy
        first.depends = $(first) copydata
        export(first.depends)
        export(copydata.commands)
        # now add it as an extra target
        QMAKE_EXTRA_TARGETS += first copydata
}
NGLPATH=$$(NGLDIR)
isEmpty(NGLPATH){ # note brace must be here
        message("including $HOME/NGL")
        include($(HOME)/NGL/UseNGL.pri)
}
else{ # note brace must be here
        message("Using custom NGL location")
        include($(NGLDIR)/UseNGL.pri)
}


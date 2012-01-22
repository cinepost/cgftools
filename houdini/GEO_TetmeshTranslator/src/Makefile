# Installation directory.
HFS		= /opt/houdini11
INSTDIR = $(HOME)/houdini11.1
DEVDIR	= $(HOME)/dev/cgftools

# List of C++ source files to build.
# SOP_Main.C registers the operators and handles the DSO-specifics.
SOURCES = \
    ./GEO_TetmeshTranslator.cpp

# Use the highest optimization level.
OPTIMIZER = -O3

# Additional include directories.
INCDIRS = \
    -I/usr/include

# Additional library directories.
#LIBDIRS = \

# Additional libraries.
#LIBS = \

# Set the plugin library name.
DSONAME = $(INSTDIR)/dso/GEO_TetmeshTranslator.so

CXXFLAGS = -g

# Include the GNU Makefile.
include $(HFS)/toolkit/makefiles/Makefile.gnu

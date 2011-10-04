# Installation directory.
INSTDIR = $(HOME)/houdini11.1

# List of C++ source files to build.
# SOP_Main.C registers the operators and handles the DSO-specifics.
SOURCES = \
    ./SIM_pbSolver.cpp

# Icons for custom node operators.
ICONS = \
    svg/DOP_cgfx_physbamsolver.svg

# Use the highest optimization level.
OPTIMIZER = -O3

# Additional include directories.
INCDIRS = \
    -I/usr/include \
    -I../interface

# Additional library directories.
LIBDIRS = \
	-L/home/max/dev/cgftools/houdini/pbSolver/libs/boost
#    -L/opt/studio/sdk/lib \
#    -L/opt/studio/third_party/sdk/lib

# Additional libraries.
LIBS = \
	-llog4cpp \
	-lboost_serialization-gcc44-mt-1_37
#	/home/max/dev/cgftools/houdini/pbSolver/libs/boost/libboost_serialization-gcc44-mt-1_37.so.1.37.0
#	/home/max/dev/cgftools/houdini/pbSolver/libs/boost/libboost_serialization.a
#	-static -llibboost_serialization
#    -lthird_partySDK

# Set the plugin library name.
DSONAME = $(INSTDIR)/dso/SIM_pbSolver.so

# Include the GNU Makefile.
include $(HFS)/toolkit/makefiles/Makefile.gnu

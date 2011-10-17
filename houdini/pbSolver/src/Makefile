# Installation directory.
INSTDIR = $(HOME)/houdini11.1
DEVDIR	= $(HOME)/dev/cgftools
PHYSBAM = /opt/PhysBAM-2011

# List of C++ source files to build.
# SOP_Main.C registers the operators and handles the DSO-specifics.
SOURCES = \
    ./SIM_pbSolver.cpp

# Icons for custom node operators.
ICONS = \
    svg/DOP_pbm_def_solver.svg

# Use the highest optimization level.
OPTIMIZER = -O3

# Additional include directories.
INCDIRS = \
    -I/usr/include \
    -I$(PHYSBAM)/Projects/deformable_interface

# Additional library directories.
LIBDIRS = \
	-L$(DEVDIR)/houdini/pbSolver/libs/boost

# Additional libraries.
LIBS = \
	-llog4cpp \
	#-lboost_serialization-gcc44-mt-1_37

# Set the plugin library name.
DSONAME = $(INSTDIR)/dso/SIM_pbSolver.so

# Include the GNU Makefile.
include $(HFS)/toolkit/makefiles/Makefile.gnu

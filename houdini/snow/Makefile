# Installation directory.
INSTDIR = $(HOME)/houdini11.1

# List of C++ source files to build.
# SOP_Main.C registers the operators and handles the DSO-specifics.
SOURCES = \
    ./SNOW_Solver.C

# Icons for custom node operators.
#ICONS = \
#    svg/DOP_pbm_def_solver.svg

# Use the highest optimization level.
OPTIMIZER = -O3

# Additional include directories.
INCDIRS = \
    -I/usr/include
    
# Additional library directories.
#LIBDIRS = \
#	-L$(DEVDIR)/houdini/pbSolver/libs/boost

# Additional libraries.
#LIBS = \
#	-llog4cxx \
	#-lboost_serialization-gcc44-mt-1_37

# Set the plugin library name.
DSONAME = $(INSTDIR)/dso/SNOW_Solver.so

# Include the GNU Makefile.
include $(HFS)/toolkit/makefiles/Makefile.gnu

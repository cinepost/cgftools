# Installation directory.
HFS		= /opt/houdini11
INSTDIR = $(HOME)/houdini11.1
DEVDIR	= $(HOME)/dev/cgftools
PHYSBAM = /opt/PhysBAM-2011

# List of C++ source files to build.
# SOP_Main.C registers the operators and handles the DSO-specifics.
SOURCES = \
	./SIM_PhysBAM_Commons.cpp \
    ./SIM_PhysBAM_Deformable_Solver.cpp \
    ./SIM_PhysBAM_WorldData.cpp \
    ./HPI_Baked_Object.cpp \
    ./HPI_Solid_Object.cpp \
	./HPI_Ground_Object.cpp \
    ./HPI_Object.cpp \
    ./HPI_Trimesh.cpp

# Icons for custom node operators.
ICONS = \
    svg/DOP_pbm_def_solver.svg \
    svg/DOP_physbam_defgeometry.svg \
    svg/DOP_physbam_visualize.svg

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
	#-llog4cxx \
	#-lboost_serialization-gcc44-mt-1_37

# Set the plugin library name.
DSONAME = $(INSTDIR)/dso/SIM_PhysBAM_Deformable_Solver.so

CXXFLAGS = -g

# Include the GNU Makefile.
include $(HFS)/toolkit/makefiles/Makefile.gnu

/*
 * Copyright (c) 2009
 *	Side Effects Software Inc.  All rights reserved.
 *
 * Redistribution and use of Houdini Development Kit samples in source and
 * binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. The name of Side Effects Software may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *----------------------------------------------------------------------------
 */

/*
 * The simplest network to see something working with this...
 *
 *  Empty Object     SnowVisualize
 *         \           /
 *         Snow VoxelArray
 *             |
 *        Snow Solver  (Birth rate .1)
 */

#ifndef __SNOW_Solver_h__
#define __SNOW_Solver_h__

#include <UT/UT_HashTable.h>
#include <UT/UT_Hash.h>
#include <UT/UT_IStream.h>
#include <UT/UT_VoxelArray.h>
#include <GU/GU_DetailHandle.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_Geometry.h>

#define SIM_NAME_BIRTHRATE	"birthrate"
#define SIM_NAME_ORIGINALDEPTH	"originaldepth"

class SIM_Object;
class GEO_Point;
class SIM_Random;

typedef unsigned char		u8;

namespace HDK_Sample {

class SNOW_VoxelArray;

// This class implemented a computational fluid dynamics solver.
class SNOW_Solver : public SIM_SingleSolver,
		       public SIM_OptionsUser
{
public:
    // Access methods for our configuration data.
    GETSET_DATA_FUNCS_F(SIM_NAME_BIRTHRATE, BirthRate);
    GETSET_DATA_FUNCS_I(SIM_NAME_ORIGINALDEPTH, OriginalDepth);
    
protected:
    explicit		 SNOW_Solver(const SIM_DataFactory *factory);
    virtual		~SNOW_Solver();

    SIM_Random		*createRandomData(SIM_Object *obj) const;

    virtual SIM_Result	 solveSingleObjectSubclass(SIM_Engine &engine,
					SIM_Object &object,
					SIM_ObjectArray &feedbacktoobjects,
					const SIM_Time &timestep,
					bool newobject);

    int			 rand_choice(int numchoice, SIM_Random *rand) const;
    bool		 brownianize(int &v, int dv, int max,
				     SIM_Random *rand) const;
    int			 clearInDirection(const SNOW_VoxelArray &snow,
				int sx, int sy, int sz,
				int dx, int dy, int dz,
				int &rx, int &ry, int &rz,
				int maxdist,
				SIM_Random *rand) const;
    void		 clearSnow(SNOW_VoxelArray &snow,
				int x, int y, int z,
				SIM_Random *rand) const;

    void		 fillRow(SNOW_VoxelArray &snow,
				fpreal startx, fpreal endx,
				int y, int z,
				u8 voxeltype,
				SIM_Random *rand) const;
    void		 applyGeometry(SNOW_VoxelArray &snow,
				const GU_ConstDetailHandle &gdh,
				const UT_DMatrix4 &xform,
				u8 voxletype,
				SIM_Random *rand) const;

private:
    static const SIM_DopDescription	*getSolverSNOWDopDescription();

    void		 solveForObject(SIM_Object &object,
					SNOW_VoxelArray &snow,
					const SIM_Time &timestep) const;
    void		 setVoxelArrayAttributes(
					SNOW_VoxelArray *voxelarray) const;

    DECLARE_STANDARD_GETCASTTOTYPE();
    DECLARE_DATAFACTORY(SNOW_Solver,
			SIM_SingleSolver,
			"SNOW Solver",
			getSolverSNOWDopDescription());
};

// Standard defines for voxel elements:
#define VOXEL_EMPTY		0
#define VOXEL_SNOW		1
#define VOXEL_COMPRESSED	2
#define VOXEL_WALL		3
#define VOXEL_OBJECT		4

#define SNOW_NAME_DIVISIONS	"div"
#define SNOW_NAME_CENTER	"t"
#define SNOW_NAME_SIZE		"size"

// This class hold an oriented bounding box tree.
class SNOW_VoxelArray : public SIM_Geometry
			// SIM_Geometry already mixes this in.
			// public SIM_OptionsUser
{
public:
    GETSET_DATA_FUNCS_V3(SNOW_NAME_DIVISIONS, Divisions);
    GETSET_DATA_FUNCS_V3(SNOW_NAME_SIZE, Size);
    GETSET_DATA_FUNCS_V3(SNOW_NAME_CENTER, Center);

    // Read an element:  This will return 0 for out of bound x/y/z values.
    u8			 getVoxel(int x, int y, int z) const;
    void		 setVoxel(u8 voxel, int x, int y, int z);

    void		 collapseAllTiles();

    void		 pubHandleModification()
			 { handleModification(); }

protected:
    explicit		 SNOW_VoxelArray(const SIM_DataFactory *factory);
    virtual		~SNOW_VoxelArray();

    // Overrides to properly implement this class as a SIM_Data.
    virtual void	 initializeSubclass();
    virtual void	 makeEqualSubclass(const SIM_Data *source);
    virtual void	 saveSubclass(ostream &os) const;
    virtual bool	 loadSubclass(UT_IStream &is);
    virtual void	 handleModificationSubclass(int code);

    virtual int64	 getMemorySizeSubclass() const;

    /// Invoked when our parameters are changed, we use it to
    /// invalidate our array.
    virtual void	 optionChangedSubclass(const char *name);

    // Override the getGeometrySubclass function to construct our geometric
    // representation in a just-in-time fashion.
    virtual GU_ConstDetailHandle getGeometrySubclass() const;

private:
    static const SIM_DopDescription	*getVoxelArrayDopDescription();

    GEO_Point		*createOrFindPoint(GU_Detail *gdp, int x, int y, int z);
    void		 buildFace(GU_Detail *gdp,
				   int x0, int y0, int z0,
				   int x1, int y1, int z1,
				   int x2, int y2, int z2,
				   int x3, int y3, int z3);
    void		 buildGeometryFromArray();
    void		 freeArray() const;
    void		 allocateArray() const;

    mutable GU_DetailHandle		 myDetailHandle;
    mutable UT_VoxelArray<u8>		*myVoxelArray;

    UT_HashTable			 myPointHash;

    DECLARE_STANDARD_GETCASTTOTYPE();
    DECLARE_DATAFACTORY(SNOW_VoxelArray,	// Our Classname
			SIM_Geometry,		// Base type
			"hdk_VoxelArray",	// DOP Data Type
			getVoxelArrayDopDescription() // PRM list.
			);
};

// This class hold an oriented bounding box tree.
class SNOW_Visualize : public SIM_Data,
			public SIM_OptionsUser
{
public:
    GET_GUIDE_FUNC_B(SIM_NAME_SHOWGUIDE, ShowGuide, true);
    GET_GUIDE_FUNC_V3(SIM_NAME_COLOR, Color, (1, 1, 1));
    GET_GUIDE_FUNC_B("usebox", UseBox, false);

protected:
    explicit		 SNOW_Visualize(const SIM_DataFactory *factory);
    virtual		~SNOW_Visualize();

    // Overrides to properly implement this class as a SIM_Data.
    virtual void	 initializeSubclass();

    virtual bool	 getIsAlternateRepresentationSubclass() const;
    virtual void	 initAlternateRepresentationSubclass(const SIM_Data &);
    virtual SIM_Guide	*createGuideObjectSubclass() const;
    virtual void	 buildGuideGeometrySubclass(const SIM_RootData &root,
						    const SIM_Options &options,
						    const GU_DetailHandle &gdh,
						    UT_DMatrix4 *xform,
						    const SIM_Time &t) const;

    static void		createBoundingBoxGuide(GU_Detail *gdp,
				    const UT_BoundingBox &bbox,
				    const UT_Vector3 &color);

private:
    static const SIM_DopDescription	*getVisualizeDopDescription();

    const SNOW_VoxelArray	*myArray;

    DECLARE_STANDARD_GETCASTTOTYPE();
    DECLARE_DATAFACTORY(SNOW_Visualize,	// Our Classname
			SIM_Data,		// Base type
			"hdk_SnowVisualize",	// DOP Data Type
			getVisualizeDopDescription() // PRM list.
			);
};

} // End the HDK_Sample namespace

#endif


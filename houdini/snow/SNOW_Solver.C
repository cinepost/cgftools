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

#include "SNOW_Solver.h"
#include <strstream.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Floor.h>
#include <UT/UT_Vector3.h>
#include <UT/UT_WorkBuffer.h>
#include <GEO/GEO_AttributeHandle.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPart.h>
#include <GU/GU_PrimPoly.h>
#include <GU/GU_RayIntersect.h>
#include <GU/GU_PrimTriStrip.h>
#include <PRM/PRM_Include.h>
#include <SIM/SIM_Engine.h>
#include <SIM/SIM_Options.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Random.h>
#include <SIM/SIM_RandomTwister.h>
#include <SIM/SIM_Position.h>
#include <SIM/SIM_PRMShared.h>
#include <SIM/SIM_Guide.h>
#include <SIM/SIM_GuideShared.h>


using namespace HDK_Sample;


SNOW_Solver::SNOW_Solver(const SIM_DataFactory *factory)
    : BaseClass(factory),
      SIM_OptionsUser(this)
{
}

SNOW_Solver::~SNOW_Solver()
{
}

const SIM_DopDescription *
SNOW_Solver::getSolverSNOWDopDescription()
{
    static PRM_Name	 theBirthRateName(SIM_NAME_BIRTHRATE, "Birth Rate");
    static PRM_Name	 theOriginalDepthName(SIM_NAME_ORIGINALDEPTH, "Original Depth");

    static PRM_Template	 theTemplates[] = {
	PRM_Template(PRM_FLT_J,		1, &theBirthRateName, PRMpointOneDefaults),
	PRM_Template(PRM_INT_J,		1, &theOriginalDepthName),
	PRM_Template()
    };

    static SIM_DopDescription	 theDopDescription(true,
						   "hdk_snowsolver",
						   "SNOW Solver",
						   SIM_SOLVER_DATANAME,
						   classname(),
						   theTemplates);

    return &theDopDescription;
}

SIM_Random *
SNOW_Solver::createRandomData(SIM_Object *obj) const
{
    SIM_Random	*rand = 0;
    
    // Create the random data as subdata attached to the solver. First
    // we look for any existing SIM_Random. If none is found, we create
    // a SIM_RandomTwister.
    rand = SIM_DATA_GET(*obj, "Random", SIM_Random);
    if( !rand )
	rand = SIM_DATA_CREATE(*obj, "Random", SIM_RandomTwister, 0);

    return rand;
}

bool
SNOW_Solver::brownianize(int &v, int dv, int max, SIM_Random *rand) const
{
    if (dv)
    {
	v += dv;
	if (v < 0 || v >= max)
	    return false;
    }
    else
    {
	v += rand_choice(3, rand) - 1;
	if (v < 0)
	    v = 0;
	if (v >= max)
	    v = max - 1;
    }

    return true;
}

int
SNOW_Solver::clearInDirection(const SNOW_VoxelArray &snow,
			int sx, int sy, int sz,
			int dx, int dy, int dz,
			int &rx, int &ry, int &rz,
			int maxdist,
			SIM_Random *rand) const
{
    int			dist = 0;
    int			xdiv, ydiv, zdiv;

    UT_Vector3		div = snow.getDivisions();
    xdiv = div.x();
    ydiv = div.y();
    zdiv = div.z();

    while (1)
    {
	// Assume our current location is invalid.  Try one step.
	if (!brownianize(sx, dx, xdiv, rand))
	    return maxdist;
	if (!brownianize(sy, dy, ydiv, rand))
	    return maxdist;
	if (!brownianize(sz, dz, zdiv, rand))
	    return maxdist;
	    
	// Now, see if we are suddenly valid.
	if (snow.getVoxel(sx, sy, sz) == VOXEL_EMPTY)
	{
	    break;
	}

	dist++;
    }

    rx = sx;
    ry = sy;
    rz = sz;
    return dist;
}

// Assumes that x,y,z had a piece of snow.  Will find a new home
// for this snow.
void
SNOW_Solver::clearSnow(SNOW_VoxelArray &snow,
			  int x, int y, int z, SIM_Random *rand) const
{
    // There are 6 primary axes for snow to be distributed along.
    // The snow tries all 6 directions, and moves in teh one that
    // has the shortest path.

    int			end_x[6], end_y[6], end_z[6], dist[6];

    int		dxvals[6] = { -1,  0,  0,  1,  0,  0 };
    int		dyvals[6] = {  0, -1,  1,  0,  0,  0 };
    int		dzvals[6] = {  0,  0,  0,  0,  1, -1 };
    int		direction, mindir, mindist = 320000;

    for (direction = 0; direction < 6; direction++)
    {
	dist[direction] =
	    clearInDirection(snow,
			x, y, z, 
			dxvals[direction], dyvals[direction], dzvals[direction],
			end_x[direction], end_y[direction], end_z[direction],
			mindist,
			rand);

	if (dist[direction] < mindist)
	{
	    mindir = direction;
	    mindist = dist[direction];
	}
    }

    if (mindist == 320000)
    {
	// Complete failure!
	UT_ASSERT(!"No snow removal possible!");
    }
    else
    {
	// Store in the resulting position...
	snow.setVoxel(VOXEL_SNOW, end_x[mindir], end_y[mindir], end_z[mindir]);
    }
}

int
SNOW_Solver::rand_choice(int numchoice, SIM_Random *rand) const
{
    int		choice;
    
    choice = rand->choice(numchoice);

    return choice;
}

void
SNOW_Solver::fillRow(SNOW_VoxelArray &snow,
			fpreal startx, fpreal endx,
			int y, int z,
			u8 voxeltype,
			SIM_Random *rand) const
{
    int		xdiv, ydiv, zdiv;
    int		x, sx, ex;

    UT_Vector3		div = snow.getDivisions();
    xdiv = div.x();
    ydiv = div.y();
    zdiv = div.z();

    sx = (int)(startx * xdiv);
    ex = (int)(endx * xdiv);
    if (sx < 0) sx = 0;
    if (sx >= xdiv) return;
    if (ex < 0) return;
    if (ex >= xdiv) ex = xdiv - 1;

    if (voxeltype == VOXEL_OBJECT)
    {
	for (x = sx; x < ex; x++)
	{
	    // Set this voxel.  TODO: Move away snow!
	    if (snow.getVoxel(x, y, z) == VOXEL_SNOW)
		clearSnow(snow, x, y, z, rand);
	    snow.setVoxel(VOXEL_OBJECT, x, y, z);
	}
    }
    else if (voxeltype == VOXEL_SNOW)
    {
	for (x = sx; x < ex; x++)
	{
	    snow.setVoxel(VOXEL_SNOW, x, y, z);
	}
    }
}

void
SNOW_Solver::applyGeometry(SNOW_VoxelArray &snow,
			      const GU_ConstDetailHandle &gdh,
			      const UT_DMatrix4 &xform,
			      u8 voxeltype,
			      SIM_Random *rand) const
{
    if( !gdh.isNull() )
    {
	GU_DetailHandleAutoReadLock	 gdl(gdh);
	const GU_Detail			*gdp = gdl.getGdp();
	int				 xdiv, ydiv, zdiv;
	int				 y, z;

	UT_Vector3		div = snow.getDivisions();
	xdiv = div.x();
	ydiv = div.y();
	zdiv = div.z();

	GU_RayIntersect		*isect;
	UT_BoundingBox		 bbox;
	GU_RayInfo		 	 hitinfo;
	int				 numhit, hitnum;
	UT_Matrix4		 fxform;

	fxform = xform;

	fxform.invert();
	gdp->getBBox(&bbox);
	bbox.transform(fxform);

	// Find where this is at least valid...
	int			 bminx, bminy, bminz;
	int			 bmaxx, bmaxy, bmaxz;
	UT_Vector3		 orig, dir, pos;
	UT_Vector3		 xorig, xdir, xpos;

	// Find the range of the bounding box - we only need
	// to search this part of the voxel array.
	bminx = (int)UTfloor(bbox(0, 0) * (xdiv + 1));
	if (bminx < 0) bminx = 0;
	bmaxx = (int)UTceil(bbox(0, 1) * (xdiv + 1));
	if (bmaxx >= xdiv) bmaxx = xdiv-1;
	bminy = (int)UTfloor(bbox(1, 0) * (ydiv + 1));
	if (bminy < 0) bminy = 0;
	bmaxy = (int)UTceil(bbox(1, 1) * (ydiv + 1));
	if (bmaxy >= ydiv) bmaxy = ydiv-1;
	bminz = (int)UTfloor(bbox(2, 0) * (zdiv + 1));
	if (bminz < 0) bminz = 0;
	bmaxz = (int)UTceil(bbox(2, 1) * (zdiv + 1));
	if (bmaxz >= zdiv) bmaxz = zdiv-1;

	// Build the ray intersect cache.
	isect = new GU_RayIntersect(gdp);
	
	// We build downwards so snow tends to
	// compact.
	orig.x() = 0.0;
	dir.assign(1.0, 0.0, 0.0);
	xdir = dir;
	xdir.multiply3(xform);
		
	for (z = bmaxz; z >= bminz; z--)
	{
	    orig.z() = (z + 0.5) / (zdiv + 1);
	    for (y = bminy; y <= bmaxy; y++)
	    {
		orig.y() = (y + 0.5) / (ydiv + 1);
		hitinfo.reset();

		xorig = orig;
		xorig *= xform;

		hitinfo.init(1.0, 0.0, GU_FIND_ALL, 1e-4);

		numhit = isect->sendRay(xorig, xdir, hitinfo);

		// -1 means interrupt from user.
		if (numhit < 0)
		    return;

		// Even if there were no hits, we may still be entirely
		// inside the object.
		numhit = hitinfo.myHitList->entries();

		fpreal		lt, t;
		
		// Now, walk through each hit...
		// First "hit" occurs at position zero.  Last "hit"
		// occurs at position 1.
		lt = 0.0;
		
		for (hitnum = 0; hitnum <= numhit; hitnum++)
		{
		    if (hitnum < numhit)
			t = (*hitinfo.myHitList)(hitnum).t;
		    else
			t = 1.0;

		    // Determine if the lt - t segment is inside or not.
		    pos = orig;
		    pos.x() = (t + lt) / 2.0;
		    xpos = pos;
		    xpos *= xform;
		    if (isect->isInsideWinding(xpos, 0))
		    {
			fillRow(snow, lt, t, y, z, voxeltype, rand);
		    }

		    lt = t;
		}
	    }
	}

	delete isect;
    }
}

void
SNOW_Solver::solveForObject(SIM_Object &object,
			      SNOW_VoxelArray &snow,
			      const SIM_Time & /*timestep*/) const
{
    // Birth new snow at top.
    int			 xdiv, ydiv, zdiv;
    int			 x, y, z;
    int			 i, n;
    fpreal		 birthrate;
    SIM_Random		*rand = createRandomData(&object);

    UT_Vector3		div = snow.getDivisions();
    xdiv = div.x();
    ydiv = div.y();
    zdiv = div.z();

    UT_Vector3	center = snow.getCenter();
    UT_Vector3	size = snow.getSize();
    UT_DMatrix4	tosnow;

    tosnow.identity();
    tosnow.scale(size.x(), size.y(), size.z());
    tosnow.pretranslate(-0.5, -0.5, -0.5);
    tosnow.translate(center.x(), center.y(), center.z());

    birthrate = getBirthRate();

    // Update according to the possibly changed intersection information.
    const SIM_Geometry	*geometry = 0;

    // First, clear out all old intersection information.
    for (z = 0; z < zdiv; z++)
    {
	for (y = 0; y <= ydiv; y++)
	{
	    for (x = 0; x <= xdiv; x++)
	    {
		if (snow.getVoxel(x, y, z) == VOXEL_OBJECT)
		    snow.setVoxel(VOXEL_EMPTY, x, y, z);
	    }
	}
    }

    // Run through each affector looking for source generators...
    SIM_ObjectArray		sourceaffectors;
    SIM_ColliderInfoArray	colliderinfo;
    UT_String			sourceobjects;

    object.getAffectors(sourceaffectors, "SIM_RelationshipSource");
    n = sourceaffectors.entries();
    for (i = 0; i < n; i++)
    {
	const SIM_Object	*affector = sourceaffectors(i);
	const SIM_Position	*pos = affector->getPosition();
	UT_DMatrix4		 xform, worldtogeo;

	geometry = affector->getGeometry();
	// Ignore people that don't have a "geometry" field.
	if (!geometry)
	    continue;

	geometry->getTransform(xform);
	xform.invert();
	if (pos)
	{
	    pos->getInverseTransform(worldtogeo);
	    xform = worldtogeo * xform;
	}

	xform = tosnow * xform;

	applyGeometry(snow, geometry->getGeometry(), xform, VOXEL_SNOW, rand);
    }

    // Run through each affector looking for geometry data...
    object.getColliderInfo(colliderinfo);
    n = colliderinfo.entries();
    for (i = 0; i < n; i++)
    {
	const SIM_Object	*affector = colliderinfo(i).getAffector();
	const SIM_Position	*pos = affector->getPosition();
	UT_DMatrix4		 xform, worldtogeo;

	geometry = affector->getGeometry();
	// Ignore people that don't have a "geometry" field.
	if (!geometry)
	    continue;

	geometry->getTransform(xform);
	xform.invert();
	if (pos)
	{
	    pos->getInverseTransform(worldtogeo);
	    xform = worldtogeo * xform;
	}
	xform = tosnow * xform;

	applyGeometry(snow, geometry->getGeometry(), xform, VOXEL_OBJECT, rand);
    }

    // Birth new snow at the top of the box.
    if (!UTequalZero(birthrate))
	for (y = 0; y < ydiv; y++)
	{
	    for (x = 0; x < xdiv; x++)
	    {
		if (rand->frandom() < birthrate)
		{
		    snow.setVoxel(VOXEL_SNOW, x, y, zdiv-1);
		}
	    }
	}

    int		dxvals[9] = { -1, -1, -1,  0,  0,  0,  1,  1,  1 };
    int		dyvals[9] = { -1,  0,  1, -1,  0,  1, -1,  0,  1 };
    int		validdxidx[9];
    int		numdxidx, dxidx;

    // And move everything down one level...
#if 1
    for (z = 1; z < zdiv; z++)
    {
	// If this snow voxel is set to 1, we want to try and move it down
	// to z-1.
	// We don't want to be too consistent with our direction or we'll
	// induce a strong bias.  Thus we reverse our loops depending
	// on z value.
	int		yend, ystart, yinc;
	int		xend, xstart, xinc;
	
	if (z & 1)
	{
	    ystart = 0;
	    yend = ydiv;
	    yinc = 1;
	    xstart = 0;
	    xend = xdiv;
	    xinc = 1;
	}
	else
	{
	    ystart = ydiv-1;
	    yend = -1;
	    yinc = -1;
	    xstart = xdiv-1;
	    xend = -1;
	    xinc = -1;
	}

	for (y = ystart; y != yend; y += yinc)
	{
	    for (x = xstart; x != xend; x += xinc)
	    {
		if (snow.getVoxel(x, y, z) == VOXEL_SNOW)
		{
		    // Try all dx combinations.
		    numdxidx = 0;
		    for (dxidx = 0; dxidx < 9; dxidx++)
		    {
			if (snow.getVoxel(x + dxvals[dxidx],
					   y + dyvals[dxidx],
					   z-1) == VOXEL_EMPTY)
			{
			    validdxidx[numdxidx++] = dxidx;
			}
		    }

		    if (numdxidx)
		    {
			dxidx = rand_choice(numdxidx, rand);

			dxidx = validdxidx[dxidx];
			
			// We can successfully move...
			snow.setVoxel(VOXEL_EMPTY, x, y, z);
			UT_ASSERT(snow.getVoxel(x + dxvals[dxidx],
						y + dyvals[dxidx],
						z-1) == VOXEL_EMPTY);
			snow.setVoxel(VOXEL_SNOW, x + dxvals[dxidx],
					 y + dyvals[dxidx],
					 z-1);
		    }
		}
	    }
	}
    }
#endif

    // Now we want to auto-collapse anything that is constant.
    snow.collapseAllTiles();
    snow.pubHandleModification();
}

void
SNOW_Solver::setVoxelArrayAttributes(SNOW_VoxelArray *voxelarray) const
{
    if( voxelarray )
    {
	int		x, y, z, depth;
	UT_Vector3	div = voxelarray->getDivisions();

	int	xdiv = div.x();
	int	ydiv = div.y();
	int	zdiv = div.z();

	depth = getOriginalDepth();

	for (z = 0; z < SYSmin(depth, zdiv); z++)
	{
	    for (y = 0; y < ydiv; y++)
	    {
		for (x = 0; x < xdiv; x++)
		{
		    voxelarray->setVoxel(VOXEL_SNOW, x, y, z);
		}
	    }
	}
	voxelarray->collapseAllTiles();
	voxelarray->pubHandleModification();
    }
}

SIM_Solver::SIM_Result
SNOW_Solver::solveSingleObjectSubclass(SIM_Engine & /*engine*/,
					  SIM_Object &object,
					  SIM_ObjectArray &,
					  const SIM_Time &timestep,
					  bool isnewobject)
{
    SNOW_VoxelArray	*snow;
    SIM_Result		 result;

    result = SIM_SOLVER_FAIL;
    // First, collect (or create) all the data we need from the object.
    snow = SIM_DATA_GET(object, "SnowValue", SNOW_VoxelArray);
    if (!snow)
    {
	snow = SIM_DATA_CREATE(object, "SnowValue", SNOW_VoxelArray, 0);
    }

    // Rebuild the snow data to the desired base level if this is a new object.
    if (snow)
    {
	if (isnewobject)
	{
	    setVoxelArrayAttributes(snow);
	    result = SIM_SOLVER_SUCCESS;
	}
	else
	{
	    solveForObject(object, *snow, timestep);
	    result = SIM_SOLVER_SUCCESS;
	}
    }

    return result;
}


SNOW_VoxelArray::SNOW_VoxelArray(const SIM_DataFactory *factory)
    : BaseClass(factory),
      myVoxelArray(0)
{
}

SNOW_VoxelArray::~SNOW_VoxelArray()
{
    freeArray();
}

const SIM_DopDescription *
SNOW_VoxelArray::getVoxelArrayDopDescription()
{
    static PRM_Name	 theDivisionsName(SNOW_NAME_DIVISIONS, "Divisions");
    static PRM_Name	 theCenterName(SNOW_NAME_CENTER, "Center");
    static PRM_Name	 theSizeName(SNOW_NAME_SIZE, "Size");

    static PRM_Template	 theTemplates[] = {
	PRM_Template(PRM_INT,		3, &theDivisionsName, PRMtenDefaults),
	PRM_Template(PRM_XYZ,		3, &theCenterName, PRMzeroDefaults),
	PRM_Template(PRM_XYZ,		3, &theSizeName, PRMoneDefaults),
	PRM_Template()
    };

    static SIM_DopDescription	 theDopDescription(true,
						   "hdk_snowvoxelarray",
						   "SNOW VoxelArray",
						   "SnowValue",
						   classname(),
						   theTemplates);

    return &theDopDescription;
}

void
SNOW_VoxelArray::optionChangedSubclass(const char *name)
{
    if (!name ||
	!strcmp(name, SNOW_NAME_DIVISIONS))
    {
	// Any current array will be invalid now.
	freeArray();
    }

    SIM_OptionsUser::optionChangedSubclass(name);
}

u8
SNOW_VoxelArray::getVoxel(int x, int y, int z) const
{
    if( !myVoxelArray )
	allocateArray();

    return myVoxelArray->getValue(x, y, z);
}

void
SNOW_VoxelArray::setVoxel(u8 voxel, int x, int y, int z)
{
    if( !myVoxelArray )
	allocateArray();

    if (myVoxelArray->isValidIndex(x, y, z))
	myVoxelArray->setValue(x, y, z, voxel);
}

GU_ConstDetailHandle
SNOW_VoxelArray::getGeometrySubclass() const
{
    ((SNOW_VoxelArray *)this)->buildGeometryFromArray();
    return myDetailHandle;
}

void
SNOW_VoxelArray::freeArray() const
{
    delete myVoxelArray;
    myVoxelArray = 0;
}

void
SNOW_VoxelArray::allocateArray() const
{
    UT_ASSERT(myVoxelArray == 0);

    myVoxelArray = new UT_VoxelArray<u8>;

    UT_Vector3		div = getDivisions();

    div.x() = SYSmax(div.x(), 1.0f);
    div.y() = SYSmax(div.y(), 1.0f);
    div.z() = SYSmax(div.z(), 1.0f);

    myVoxelArray->size(div.x(), div.y(), div.z());

    // We want out of bound values to evaluate to wall voxels.
    myVoxelArray->setBorder(UT_VOXELBORDER_CONSTANT, VOXEL_WALL);
}

GEO_Point *
SNOW_VoxelArray::createOrFindPoint(GU_Detail *gdp, int x, int y, int z)
{
    int		idx;
    UT_Thing	thing;
    GEO_Point	*pt;
    UT_Vector3		div = getDivisions();
    int		xdiv = div.x();
    int		ydiv = div.y();
    int		zdiv = div.z();

    idx = (z * (ydiv + 1) + y)*(xdiv + 1) + x;

    UT_Hash_Int	hash(idx);

    if (myPointHash.findSymbol(hash, &thing))
    {
	return (GEO_Point *) thing.value.voidp;
    }

    // Create and add!

    pt = gdp->appendPoint();

    UT_Vector3		v4;
    v4.assign((fpreal) x / (fpreal) (xdiv + 1), 
	      (fpreal) y / (fpreal) (ydiv + 1), 
	      (fpreal) z / (fpreal) (zdiv + 1));

    v4 -= 0.5;
    v4 *= getSize();
    v4 += getCenter();

    pt->getPos() = v4;

    // And add to the hash...
    thing.value.voidp = pt;
    myPointHash.addSymbol(hash, thing);

    return pt;
}

void		 
SNOW_VoxelArray::buildFace(GU_Detail *gdp,
			   int x0, int y0, int z0,
			   int x1, int y1, int z1,
			   int x2, int y2, int z2,
			   int x3, int y3, int z3)
{
    // Triangle strip code.
    GU_PrimTriStrip	*tristrip;
    GEO_Point		*pt;

    // Do not append points, as we may scavenge.
    tristrip = GU_PrimTriStrip::build(gdp, 4, 0);

    pt = createOrFindPoint(gdp, x0, y0, z0);
    (*tristrip)(0).setPt(pt);
    pt = createOrFindPoint(gdp, x1, y1, z1);
    (*tristrip)(1).setPt(pt);
    pt = createOrFindPoint(gdp, x3, y3, z3);
    (*tristrip)(2).setPt(pt);
    pt = createOrFindPoint(gdp, x2, y2, z2);
    (*tristrip)(3).setPt(pt);
}

void
SNOW_VoxelArray::buildGeometryFromArray()
{
    if( myDetailHandle.isNull() )
    {
	GU_Detail	*gdp = new GU_Detail();
	int		 x, y, z;
	int		 xstep, ystep, zstep;

	UT_Vector3	div = getDivisions();
	int		xdiv = div.x();
	int		ydiv = div.y();
	int		zdiv = div.z();

	myDetailHandle.allocateAndSet(gdp);

	// Find the appropriate step value...
	xstep = ystep = zstep = 1;
	if (xdiv > 64)
	    xstep = xdiv / 64;
	if (ydiv > 64)
	    ystep = ydiv / 64;
	if (zdiv > 64)
	    zstep = zdiv / 64;
	
	for( z = 0; z < zdiv; z+=zstep )
	{
	    for( y = 0; y < ydiv; y+=ystep )
	    {
		for( x = 0; x < xdiv; x+=xstep )
		{
		    if (getVoxel(x, y, z) == VOXEL_SNOW)
		    {
			// Check each of the cardinal directions
			// to see if we want to build a face.

			// We want to render the faces of this cube
			// that are bordered by an empty unit.
			// We specify the points as (x,y,z) triplets.
			// This cube is (x,y,z) to (x+1,y+1,z+1)
			if (getVoxel(x-xstep, y, z) != VOXEL_SNOW)
			{
			    buildFace(	gdp, x, y, z,
					x, y+ystep, z,
					x, y+ystep, z+zstep,
					x, y, z+zstep );
			}
			if (getVoxel(x+xstep, y, z) != VOXEL_SNOW)
			{
			    buildFace(	gdp, x+xstep, y, z,
					x+xstep, y, z+zstep,
					x+xstep, y+ystep, z+zstep,
					x+xstep, y+ystep, z );
			}
			if (getVoxel(x, y-ystep, z) != VOXEL_SNOW)
			{
			    buildFace(	gdp, x, y, z,
					x, y, z+zstep,
					x+xstep, y, z+zstep,
					x+xstep, y, z );
			}
			if (getVoxel(x, y+ystep, z) != VOXEL_SNOW)
			{
			    buildFace(	gdp, x, y+ystep, z,
					x+xstep, y+ystep, z,
					x+xstep, y+ystep, z+zstep,
					x, y+ystep, z+zstep );
			}
			if (getVoxel(x, y, z-zstep) != VOXEL_SNOW)
			{
			    buildFace(	gdp, x, y, z,
					x+xstep, y, z,
					x+xstep, y+ystep, z,
					x, y+ystep, z );
			}
			if (getVoxel(x, y, z+zstep) != VOXEL_SNOW)
			{
			    buildFace(	gdp, x, y, z+zstep,
					x, y+ystep, z+zstep,
					x+xstep, y+ystep, z+zstep,
					x+xstep, y, z+zstep );
			}
		    }
		}
	    }
	}

	// Wipe out all the points we allocated.
	myPointHash.clear();
    }
}

void
SNOW_VoxelArray::initializeSubclass()
{
    BaseClass::initializeSubclass();
    freeArray();
    myDetailHandle.clear();
}

void
SNOW_VoxelArray::makeEqualSubclass(const SIM_Data *source)
{
    const SNOW_VoxelArray	*srcvox;

    BaseClass::makeEqualSubclass(source);
    srcvox = SIM_DATA_CASTCONST(source, SNOW_VoxelArray);
    if( srcvox )
    {
	setDivisions(srcvox->getDivisions());
	
	if (srcvox->myVoxelArray)
	{
	    // Copy over the voxels.
	    allocateArray();

	    *myVoxelArray  = *srcvox->myVoxelArray;
	}
	else
	{
	    // No voxel array, so nothing to copy.
	    freeArray();
	}
    }
}

void
SNOW_VoxelArray::saveSubclass(ostream &os) const
{
    SIM_Options		 voxeldata;
    int			 x, y, z, value;
    UT_Vector3		 div = getDivisions();
    int			 xdiv = div.x();
    int			 ydiv = div.y();
    int			 zdiv = div.z();

    BaseClass::saveSubclass(os);

    os << "{" << endl;
    for( z = 0; z < zdiv; z++ )
    {
	for( y = 0; y < ydiv; y++ )
	{
	    os << "\t";
	    for( x = 0; x < xdiv; x++ )
	    {
		value = getVoxel(x, y, z);
		os << " " << value;
	    }
	    os << endl;
	}
    }
    os << "}" << endl;
}

bool
SNOW_VoxelArray::loadSubclass(UT_IStream &is)
{
    SIM_Options		 voxeldata;
    UT_WorkBuffer	 buf;
    int			 idx, arraysize;
    int			 x, y, z;
    int			 value;
    bool		 result = true;

    if (!BaseClass::loadSubclass(is))
	return false;

    UT_Vector3		 div = getDivisions();
    int			 xdiv = div.x();
    int			 ydiv = div.y();
    int			 zdiv = div.z();

    arraysize = xdiv * ydiv * zdiv;
    idx = 0;
    x = y = z = 0;
    if( is.getLine(buf) && *buf.buffer() == '{' )
    {
	while( is.getLine(buf) && *buf.buffer() != '}' )
	{
	    istrstream	 bufis(buf.lock(), buf.length());

	    while( idx < arraysize && bufis )
	    {
		if( (bufis >> value) )
		{
		    setVoxel(value, x, y, z);
		    x++;
		    if (x >= xdiv)
		    {
			x = 0;
			y++;
			if (y >= ydiv)
			{
			    y = 0;
			    z++;
			}
		    }
		}
	    }
	    buf.release();
	}
	UT_ASSERT(idx == arraysize);
    }

    return result;
}

int64
SNOW_VoxelArray::getMemorySizeSubclass() const
{
    int64		mem;

    mem = sizeof(*this);
    if (myVoxelArray)
	mem += myVoxelArray->getMemoryUsage();
    if (!myDetailHandle.isNull())
    {
	GU_DetailHandleAutoReadLock	gdl(myDetailHandle);
	const GU_Detail	*gdp = gdl.getGdp();

	mem += gdp->getMemoryUsage();
    }

    return mem;
}

void
SNOW_VoxelArray::handleModificationSubclass(int code)
{
    BaseClass::handleModificationSubclass(code);

    // Ensure we rebuild our display proxy geometry.
    myDetailHandle.clear();
}

void
SNOW_VoxelArray::collapseAllTiles()
{
    if (!myVoxelArray)
	return;

    myVoxelArray->collapseAllTiles();
}

SNOW_Visualize::SNOW_Visualize(const SIM_DataFactory *factory)
    : BaseClass(factory),
      SIM_OptionsUser(this)
{
    myArray = 0;
}

SNOW_Visualize::~SNOW_Visualize()
{
}

const SIM_DopDescription *
SNOW_Visualize::getVisualizeDopDescription()
{
    static PRM_Name	 theGuideBox("usebox", "Bounding Box");

    static PRM_Template	 theTemplates[] = {
	PRM_Template()
    };

    static PRM_Template		 theGuideTemplates[] = {
	PRM_Template(PRM_TOGGLE,	1, &SIMshowguideName,
					PRMzeroDefaults),
	PRM_Template(PRM_RGB,		3,
					&SIMcolorName, PRMoneDefaults,
					0, &PRMunitRange),
	PRM_Template(PRM_TOGGLE,	1, &theGuideBox, PRMzeroDefaults),
	PRM_Template()
    };

    static SIM_DopDescription	 theDopDescription(true,
						   "hdk_snowvisualize",
						   "SNOW Visualize",
						   "Visualization",
						   classname(),
						   theTemplates);
    theDopDescription.setGuideTemplates(theGuideTemplates);

    return &theDopDescription;
}

SIM_Guide *
SNOW_Visualize::createGuideObjectSubclass() const
{
    // Return a shared guide so that we only have to build our geometry
    // once. But set the displayonce flag to false so that we can set
    // a different transform for each object.
    return new SIM_GuideShared(this, false);
}

void
SNOW_Visualize::createBoundingBoxGuide(GU_Detail *gdp,
		const UT_BoundingBox &bbox,
		const UT_Vector3 &color)
{
    GEO_Point	*corners[8];
    int		 idx1, idx2;
    UT_Vector3	 pos;
    GU_PrimPoly	*line;
    GEO_AttributeHandle	cd_gah;

    cd_gah = gdp->getPointAttribute("Cd");
    if (!cd_gah.isAttributeValid())
    {
	static float one[3] = { 1, 1, 1 };
	gdp->addPointAttrib("Cd", 3 * sizeof(float), GB_ATTRIB_FLOAT, one);
	cd_gah = gdp->getPointAttribute("Cd");
    }

    GB_PrimitiveGroup	*bboxgrp;

    bboxgrp = gdp->newPrimitiveGroup("bbox");

    // Create the 8 points with the rule that
    // we use max(axis) if idx1 & (1 << axis) is true.
    for (idx1 = 0; idx1 < 8; idx1++)
    {
	corners[idx1] = gdp->appendPoint();

	pos.x() = (idx1 & 1) ? bbox.xmax() : bbox.xmin();
	pos.y() = (idx1 & 2) ? bbox.ymax() : bbox.ymin();
	pos.z() = (idx1 & 4) ? bbox.zmax() : bbox.zmin();
	corners[idx1]->getPos() = pos;

	cd_gah.setElement(corners[idx1]);
	cd_gah.setV3(color);
    }

    // Create each edge.  Edges are in increaing direction of index.
    // Two indicies are connected if they differ in one and only one
    // axis.  Brute force and ignorance saves the day.
    // We run the n^2 possible indices and only build if the bit
    // field of differences matches a desired pattern.
    for (idx1 = 0; idx1 < 8; idx1++)
    {
	for (idx2 = idx1+1; idx2 < 8; idx2++)
	{
	    switch (idx1 ^ idx2)
	    {
		case 1:
		case 2:
		case 4:
		    line = GU_PrimPoly::build(gdp, 2, GU_POLY_OPEN, 0);
		    bboxgrp->add(line);

		    (*line)(0).setPt(corners[idx1]);
		    (*line)(1).setPt(corners[idx2]);

		    break;
	    }
	}
    }
}

void
SNOW_Visualize::buildGuideGeometrySubclass(const SIM_RootData &root,
				    const SIM_Options &options,
				    const GU_DetailHandle &gdh,
				    UT_DMatrix4 *,
				    const SIM_Time &) const
{
    // Build our template geometry, if we are so asked.
    initAlternateRepresentation();

    if (!myArray)
	return;

    if (gdh.isNull())
	return;

    GU_DetailHandleAutoWriteLock	 gdl(gdh);
    GU_Detail				*gdp = gdl.getGdp();
    UT_BoundingBox			 bbox;
    int					 divx, divy, divz;
    int					 x, y, z;
    UT_Vector3				 color(1,1,1), div;
    GEO_AttributeHandle			 cd_gah;

    color = getColor(options);

    // Find our bounding box.
    UT_Vector3				bbmin, bbmax;

    bbmin = myArray->getCenter();
    bbmax = bbmin;
    bbmin -= myArray->getSize()*0.5;
    bbmax += myArray->getSize()*0.5;
    bbox.initBounds(bbmin, bbmax);

    cd_gah = gdp->getPointAttribute("Cd");
    if (!cd_gah.isAttributeValid())
    {
	static float one[3] = { 1, 1, 1 };
	gdp->addPointAttrib("Cd", 3 * sizeof(float), GB_ATTRIB_FLOAT, one);
	cd_gah = gdp->getPointAttribute("Cd");
    }

    div = myArray->getDivisions();
    divx = div.x();
    divy = div.y();
    divz = div.z();

    GU_PrimParticle		*part;

    // Build our particle system to hold all of our snow voxels.
    part = GU_PrimParticle::build(gdp, 0);

    for (z = 0; z < divz; z++)
	for (y = 0; y < divy; y++)
	    for (x = 0; x < divx; x++)
	    {
		if (myArray->getVoxel(x, y, z) == VOXEL_SNOW)
		{
		    GEO_Point		*pt;

		    pt = gdp->appendPoint();
		    cd_gah.setElement(pt);
		    cd_gah.setV3(color);

		    UT_Vector3		v4;
		    v4.assign((fpreal) x / (fpreal) (divx + 1), 
			      (fpreal) y / (fpreal) (divy + 1), 
			      (fpreal) z / (fpreal) (divz + 1));

		    v4 -= 0.5;
		    v4 *= myArray->getSize();
		    v4 += myArray->getCenter();

		    pt->getPos() = v4;

		    // Add this free floating point to our particle system.
		    part->appendParticle(pt);
		}
	    }


    if (getUseBox(options))
	createBoundingBoxGuide(gdp, bbox, color);
}

void
SNOW_Visualize::initializeSubclass()
{
    BaseClass::initializeSubclass();

    myArray = 0;
}

bool
SNOW_Visualize::getIsAlternateRepresentationSubclass() const
{
    return true;
}

void
SNOW_Visualize::initAlternateRepresentationSubclass(const SIM_Data &parent)
{
    const SNOW_VoxelArray	*sf = SIM_DATA_CASTCONST(&parent, SNOW_VoxelArray);

    myArray = sf;
}

void
initializeSIM(void *)
{
    IMPLEMENT_DATAFACTORY(SNOW_VoxelArray);
    IMPLEMENT_DATAFACTORY(SNOW_Visualize);
    IMPLEMENT_DATAFACTORY(SNOW_Solver);
}


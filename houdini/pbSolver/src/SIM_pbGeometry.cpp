#include "SIM_pbGeometry.h"

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

#include "logtools.h"

SIM_pbGeometry::SIM_pbGeometry(const SIM_DataFactory *factory)
    : SIM_Geometry(factory), mesh(0)
{

}

SIM_pbGeometry::~SIM_pbGeometry()
{
	delete mesh;
}

HPI_TriMesh*
SIM_pbGeometry::getMesh(){
	return mesh;
}

void
SIM_pbGeometry::setMesh(HPI_TriMesh* inmesh){
	mesh = inmesh;
}	

GU_ConstDetailHandle
SIM_pbGeometry::getGeometrySubclass() const
{
    if( myDetailHandle.isNull() )
    {
        GU_Detail* gdp = new GU_Detail();

        // Generate geometry for myOwnRepresentation and store this geometry in gdp
        gdp->polymeshCube (5, 5, 5, -.5F, .5F, -.5F, .5F, -.5F, .5F, GEO_PATCH_TRIANGLE, true);
    
		myDetailHandle.allocateAndSet(gdp);
    }
    return myDetailHandle;
}

void
SIM_pbGeometry::initializeSubclass()
{
    SIM_Geometry::initializeSubclass();
    mesh	= 0;
    myDetailHandle.clear();
}

void
SIM_pbGeometry::makeEqualSubclass(const SIM_Data *source)
{
    const SIM_pbGeometry	*srcgeo;

    SIM_Geometry::makeEqualSubclass(source);
    srcgeo = SIM_DATA_CASTCONST(source, SIM_pbGeometry);
    if( srcgeo )
    {	
		mesh			= srcgeo->mesh;
		myDetailHandle 	= srcgeo->myDetailHandle;	
    }
}

int64
SIM_pbGeometry::getMemorySizeSubclass() const
{
    int64		mem;

    mem = sizeof(*this);
    if (!myDetailHandle.isNull())
    {
		GU_DetailHandleAutoReadLock	gdl(myDetailHandle);
		const GU_Detail	*gdp = gdl.getGdp();
		mem += gdp->getMemoryUsage();
    }
    return mem;
}

void
SIM_pbGeometry::handleModificationSubclass(int code)
{
    SIM_pbGeometry::handleModificationSubclass(code);

    // Ensure we rebuild our display proxy geometry.
    myDetailHandle.clear();
}

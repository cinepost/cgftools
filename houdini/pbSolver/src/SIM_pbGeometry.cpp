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
    : BaseClass(factory), SIM_OptionsUser(this), mesh(0)
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
SIM_pbGeometry::getGeometry () const{
	return getGeometrySubclass();	
}	

GU_ConstDetailHandle
SIM_pbGeometry::getGeometrySubclass() const
{
    if( myDetailHandle.isNull() )
    {
		GU_Detail* 	gdp = new GU_Detail();
		UT_String	sop_path;
		getSopPath(sop_path);
		
		SOP_Node 	*sop_node = OPgetDirector()->findSOPNode( sop_path );
		if(sop_node){
			//const SIM_Engine	engine 	= getEngine();
			const SIM_Time		time	= getEngine().getSimulationTime(); 
			OP_Context			context(time);
			gdp->duplicate(*sop_node->getCookedGeo(context));
		}else{		
			gdp->polymeshCube(4, 4, 4, -0.5F, 0.5F, -0.5F, 0.5F, -0.5F, 0.5F, GEO_PATCH_QUADS, true);	
		}	
		
		myDetailHandle.allocateAndSet(gdp);
    }
    return myDetailHandle;
}

void
SIM_pbGeometry::initializeSubclass()
{
	LOG_INDENT;
	LOG("SIM_pbGeometry::initializeSubclass() called");
		
    SIM_Data::initializeSubclass();
    mesh	= 0;
    myDetailHandle.clear();
    
	LOG("Done.");
	LOG_UNDENT;
}

void
SIM_pbGeometry::makeEqualSubclass(const SIM_Data *source)
{
    const SIM_pbGeometry	*srcgeo;

    SIM_Data::makeEqualSubclass(source);
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
    BaseClass::handleModificationSubclass(code);

    // Ensure we rebuild our display proxy geometry.
    myDetailHandle.clear();
}

SIM_Guide *
SIM_pbGeometry::createGuideObjectSubclass() const
{
    // Return a shared guide so that we only have to build our geometry once. But set the displayonce flag to false so that we can set a different transform for each object.
	return new SIM_GuideShared(this, false);
}

void
SIM_pbGeometry::buildGuideGeometrySubclass(const SIM_RootData &root,
                                    const SIM_Options &options,
                                    const GU_DetailHandle &gdh,
                                    UT_DMatrix4 *,
                                    const SIM_Time &) const
{
	LOG_INDENT;
	LOG("SIM_pbGeometry::buildGuideGeometrySubclass() called. With self: " << this);
	
	GU_DetailHandleAutoWriteLock        gdl(gdh);
    GU_Detail                           *gdp = gdl.getGdp();
    
    if(!gdp){
		gdp = new GU_Detail();
	}
		
	gdp->duplicate(*getGeometry().readLock());
    
    LOG("Done.");
	LOG_UNDENT;		
}    

const SIM_DopDescription *
SIM_pbGeometry::getDopDescription()
{
	static PRM_Name	theGuideBox("usebox", "Bounding Box");
    static PRM_Name theMassName(NAME_MASS, "Mass");
    static PRM_Name sopPathName(NAME_SOP_PATH, "SOP Path");
    
    static PRM_Template theTemplates[] = {
		PRM_Template(PRM_TOGGLE,	1, &SIMshowguideName, PRMzeroDefaults),
		PRM_Template(PRM_RGB,		3, &SIMcolorName,	PRMoneDefaults, 0, &PRMunitRange),	
		PRM_Template(PRM_TOGGLE,	1, &theGuideBox,	PRMzeroDefaults),		
        PRM_Template(PRM_FLT_J, 	1, &theMassName,	PRMzeroDefaults),
		PRM_Template(PRM_STRING,	PRM_TYPE_DYNAMIC_PATH, 1, &sopPathName, 0, 0, 0, 0, &PRM_SpareData::sopPath),
        PRM_Template()
    };

    static SIM_DopDescription theDopDescription(true,
						   "physbam_geometry",
						   "PhysBAM Geometry",
						   "PhysBAM_Geometry",
                        classname(),
                        theTemplates);

    return &theDopDescription;
}

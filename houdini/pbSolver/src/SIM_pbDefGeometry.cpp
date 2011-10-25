#include "SIM_pbDefGeometry.h"

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

static PRM_Default     deformableStiffnessDefault(1e3);
static PRM_Default     deformableDampingDefault(.01);

SIM_pbDefGeometry::SIM_pbDefGeometry(const SIM_DataFactory *factory)
    : SIM_pbGeometry(factory)
{

}

SIM_pbDefGeometry::~SIM_pbDefGeometry()
{

}

bool
SIM_pbDefGeometry::addGeometryToSimulation(physbam_simulation *simulation){
	return true;
}

const SIM_DopDescription *
SIM_pbDefGeometry::getDopDescription()
{	
	static PRM_Name	 theStiffnessName("stiffness", "Stiffness");
    static PRM_Name	 theDampingName("damping", "Damping");

    static PRM_Template	 theTemplates[] = {
	PRM_Template(PRM_FLT,		1, &theStiffnessName, 	&deformableStiffnessDefault),
	PRM_Template(PRM_FLT,		1, &theDampingName, 	&deformableDampingDefault),
	PRM_Template()
    };

    static SIM_DopDescription	 theDopDescription(true,
						   "physbam_defgeometry",
						   "PhysBAM Deformable Geometry",
						   "PhysBAM_Geometry",
						   classname(),
						   theTemplates);	   

    return &theDopDescription;
}

void
SIM_pbDefGeometry::optionChangedSubclass(const char *name)
{
    if (!name || !strcmp(name, "damping") || !strcmp(name, "stiffness"))
    {
		// Any current array will be invalid now.
		//freeArray();
    }

    SIM_OptionsUser::optionChangedSubclass(name);
}

void
SIM_pbDefGeometry::initializeSubclass()
{
    BaseClass::initializeSubclass();
    mesh	= 0;
    myDetailHandle.clear();
}

void
SIM_pbDefGeometry::makeEqualSubclass(const SIM_Data *source)
{
    const SIM_pbDefGeometry	*srcgeo;

    BaseClass::makeEqualSubclass(source);
    srcgeo = SIM_DATA_CASTCONST(source, SIM_pbDefGeometry);
    if( srcgeo )
    {	
		mesh			= srcgeo->mesh;
		myDetailHandle 	= srcgeo->myDetailHandle;	
    }
}

void
SIM_pbDefGeometry::saveSubclass(ostream &os) const
{
	const UT_Options	opts;
    BaseClass::saveSubclass(os);
    myDetailHandle.readLock()->save(os, 1, &opts);
}

bool
SIM_pbDefGeometry::loadSubclass(UT_IStream &is)
{
	if (!BaseClass::loadSubclass(is))
	return false;

	const UT_Options	opts;
	myDetailHandle.writeLock()->load(is, &opts);

	return true;
}

int64
SIM_pbDefGeometry::getMemorySizeSubclass() const
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
SIM_pbDefGeometry::handleModificationSubclass(int code)
{
    BaseClass::handleModificationSubclass(code);

    // Ensure we rebuild our display proxy geometry.
    myDetailHandle.clear();
}

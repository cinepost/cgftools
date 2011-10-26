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
    : BaseClass(factory), SIM_OptionsUser(this)
{

}

SIM_pbDefGeometry::~SIM_pbDefGeometry()
{

}

const SIM_DopDescription *
SIM_pbDefGeometry::getDopDescription()
{	
	static PRM_Name	 theStiffnessName(NAME_STIFFNESS, "Stiffness");
    static PRM_Name	 theDampingName(NAME_DAMPING, "Damping");

    static PRM_Template	 theTemplates[] = {
	PRM_Template(PRM_FLT,		1, &theStiffnessName, 	&deformableStiffnessDefault),
	PRM_Template(PRM_FLT,		1, &theDampingName, 	&deformableDampingDefault),
	PRM_Template()
    };

    static SIM_DopDescription	 theDopDescription(true,
						   "physbam_deformable",
						   "PhysBAM Deformable",
						   "PhysBAM_Deformable",
						   classname(),
						   theTemplates);	   

    return &theDopDescription;
}

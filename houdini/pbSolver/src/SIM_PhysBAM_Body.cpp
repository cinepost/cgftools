#include "SIM_PhysBAM_Body.h"

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

SIM_PhysBAM_Body::SIM_PhysBAM_Body(const SIM_DataFactory *factory)
 : SIM_Data(factory), SIM_OptionsUser(this)
{
}

SIM_PhysBAM_Body::~SIM_PhysBAM_Body()
{
}

void
SIM_PhysBAM_Body::initializeSubclass()
{
    SIM_Data::initializeSubclass();
}

int
SIM_PhysBAM_Body::numOptions(void){
	return getOptions().getNumOptions();
}

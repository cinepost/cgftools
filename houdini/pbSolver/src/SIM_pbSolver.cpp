#include "SIM_pbSolver.h"

#include <iostream.h>
#include <string.h>
#include <strstream.h>
#include <CH/CH_LocalVariable.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Floor.h>
#include <UT/UT_Vector3.h>
#include <UT/UT_WorkBuffer.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_IStream.h>
#include <UT/UT_String.h>
#include <GEO/GEO_AttributeHandle.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPart.h>
#include <GU/GU_PrimPoly.h>
#include <GU/GU_RayIntersect.h>
#include <GU/GU_PrimTriStrip.h>
#include <PRM/PRM_Include.h>
#include <OP/OP_Parameters.h>
#include <OP/OP_OperatorTable.h>
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
#include <SIM/SIM_Solver.h>
 
void
initializeSIM(void *)
{
    IMPLEMENT_DATAFACTORY(SIM_pbSolver);
}
	
SIM_pbSolver::SIM_pbSolver(const SIM_DataFactory *factory)
    : BaseClass(factory),
      SIM_OptionsUser(this)
{
}

SIM_pbSolver::~SIM_pbSolver()
{
}

const SIM_DopDescription* SIM_pbSolver::getSolverDopDescription()
{
    static PRM_Name      theBirthRateName(SIM_NAME_BIRTHRATE, "Birth Rate");
    static PRM_Name      theOriginalDepthName(SIM_NAME_ORIGINALDEPTH, "Original Depth");

    static PRM_Template  theTemplates[] = {
        PRM_Template(PRM_FLT_J,         1, &theBirthRateName, PRMpointOneDefaults),
        PRM_Template()
    };

    static SIM_DopDescription    theDopDescription(true,
                                                   "cgfx_physbamsolver",
                                                   "CGFX PhysBAM Solver",
                                                   SIM_SOLVER_DATANAME,
                                                   classname(),
                                                   theTemplates);

    return &theDopDescription;
}	
	
SIM_Solver::SIM_Result
SIM_pbSolver::solveObjectsSubclass (
	SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects,
	SIM_ObjectArray &feedbacktoobjects,
	const SIM_Time &timestep
){
	SIM_ObjectArray      filtered;
	UT_String            group;
	int                  i;
	
	// Loop through new objects.
	if (newobjects.entries() > 0) {
		std::cout << "Setting up new objects in sim:" << std::endl; 
		for( i = 0; i < newobjects.entries(); i++ ){	
			std::cout << newobjects(i)->getName() << std::endl;
	        //setCurrentObject(i, filtered.entries(), filtered(i));
		}	
	}

	// Loop through all the objects.
	if (objects.entries() > 0){
		std::cout << "Processing objects in sim:" << std::endl; 
		for( i = 0; i < objects.entries(); i++ ){   
	        std::cout << objects(i)->getName() << std::endl;
	        //setCurrentObject(i, filtered.entries(), filtered(i));
		}	
	}
	
	//if( something went wrong )
	//   return SIM_Solver::SIM_SOLVER_FAIL;
	
	return SIM_Solver::SIM_SOLVER_SUCCESS;
}		

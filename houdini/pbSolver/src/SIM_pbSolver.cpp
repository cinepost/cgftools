#include "SIM_pbSolver.h"

#include <iostream.h>
#include <string.h>
#include <strstream.h>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/SimpleLayout.hh>
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
#include <SIM/SIM_DataFilter.h>
#include <SIM/SIM_Engine.h>
#include <SIM/SIM_Options.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Random.h>
#include <SIM/SIM_RandomTwister.h>
#include <SIM/SIM_Position.h>
#include <SIM/SIM_PRMShared.h>
#include <SIM/SIM_Guide.h>
#include <SIM/SIM_GuideShared.h>
#include <SIM/SIM_Solver.h>
#include <SIM/SIM_DataFilter.h>
#include <SIM/SIM_ConstraintIterator.h>
#include <RBD/RBD_State.h>

#define		DEBUG
#define		LOGFILE "/home/max/dev/test.log"

void
initializeSIM(void *){
    IMPLEMENT_DATAFACTORY(SIM_pbSolver);
}
	
SIM_pbSolver::SIM_pbSolver(const SIM_DataFactory *factory) : BaseClass(factory), SIM_OptionsUser(this){

}

SIM_pbSolver::~SIM_pbSolver(){

}

const SIM_DopDescription* SIM_pbSolver::getSolverDopDescription(){
    static PRM_Name      theBirthRateName(SIM_NAME_BIRTHRATE, "Birth Rate");
    static PRM_Name      theOriginalDepthName(SIM_NAME_ORIGINALDEPTH, "Original Depth");

    static PRM_Template  theTemplates[] = {
        PRM_Template(PRM_FLT_J,         1, &theBirthRateName, PRMpointOneDefaults),
        PRM_Template()
    };

    static SIM_DopDescription    theDopDescription(true, "cgfx_physbamsolver", "CGFX PhysBAM Solver", SIM_SOLVER_DATANAME, classname(), theTemplates);
    return &theDopDescription;
}	

bool SIM_pbSolver::setupNewSimObject(SIM_Object* object){
	std::cout << "\t" << object->getName() << std::endl;
	return true;
}
bool SIM_pbSolver::updateSimObject(SIM_Object* object){
	std::cout << "\t" << object->getName() << std::endl;
	
	// Get the object's last state before this time step
	const SIM_Geometry* const geometry(object->getGeometry());
	
	// Extract simulation state from geome
	RBD_State *rbdstate = SIM_DATA_GET(*object, "Position", RBD_State);

	
	// Integrate simulation state forward by time step using the solver parameters, e.g., getMyOwnAccuracy()
	std::cout << "\t\tpos: " << rbdstate->getPosition() << std::endl;
	std::cout << "\t\trot: " << rbdstate->getOrientation() << std::endl;
	std::cout << "\t\tpvt: " << rbdstate->getPivot() << std::endl;
	
	SIM_GeometryCopy* geometry_copy(
		SIM_DATA_CREATE(
			*object, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy,
			SIM_DATA_RETURN_EXISTING | SIM_DATA_ADOPT_EXISTING_ON_DELETE
		)
	);	
	
	// Store the integrated simulation state in geometry_copy
	
	return true;
}
	
SIM_Solver::SIM_Result
SIM_pbSolver::solveObjectsSubclass ( SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep){
	SIM_ObjectArray		filtered;
	SIM_Object			*object, *new_object;
	UT_String			group;
	int					i;
	
	const SIM_DataFilterByType hard_filter 	= SIM_DataFilterByType("SIM_ConRelHard");
	const SIM_DataFilterByType pin_filter 	= SIM_DataFilterByType("SIM_ConAnchorObjPointPos");
	const SIM_DataFilterByType goal_filter 	= SIM_DataFilterByType("SIM_ConAnchorWorldSpacePos");
	
	const fpreal t_end = engine.getSimulationTime();
	
	// Loop through new objects.
	if (newobjects.entries() > 0) {
		std::cout << "Setting up new objects in sim:" << std::endl; 
		for( i = 0; i < newobjects.entries(); i++ ){	
			new_object = newobjects(i);
	        //setCurrentObject(i, filtered.entries(), filtered(i));			
			setupNewSimObject(new_object);
		}	
	}

	// Loop through all the objects.
	if (objects.entries() > 0){
		std::cout << "Processing objects in sim:" << std::endl; 
		for( i = 0; i < objects.entries(); i++ ){ 
			object = objects(i);
	        //setCurrentObject(i, filtered.entries(), filtered(i));			
			updateSimObject(object);  
		}	
	}
	
	//if( something went wrong )
	//   return SIM_Solver::SIM_SOLVER_FAIL;
	
	return SIM_Solver::SIM_SOLVER_SUCCESS;
}		

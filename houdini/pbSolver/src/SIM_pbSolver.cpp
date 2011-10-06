#include "SIM_pbSolver.h"

#include <fstream>
#include <vector>
#include <iostream.h>
#include <string.h>
#include <strstream.h>

#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "interface/deformable_body_simulation.h"

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
#include <OP/OP_Operator.h>
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

using namespace data_exchange;

static PRM_Name		theSimeFileName("simfilename", "Simulation file");
static PRM_Default	theSimeFileNameDefault(0, "$HIH/projects/simfile.$SF4.pbs");

void
initializeSIM(void *){
    IMPLEMENT_DATAFACTORY(SIM_pbSolver);
	std::cout << "initializeSIM called." << std::endl;
}
	
SIM_pbSolver::SIM_pbSolver(const SIM_DataFactory *factory) : BaseClass(factory), SIM_OptionsUser(this){
	
}

SIM_pbSolver::~SIM_pbSolver(){
	
}

const SIM_DopDescription* SIM_pbSolver::getSolverDopDescription(){
    static PRM_Template  theTemplates[] = {
        PRM_Template(PRM_FILE,         1, &theSimeFileName, &theSimeFileNameDefault),
        PRM_Template()
    };

    static SIM_DopDescription    theDopDescription(true, "pbm_def_solver", "PhysBAM Deformable Solver", SIM_SOLVER_DATANAME, classname(), theTemplates);
    return &theDopDescription;
}	

bool SIM_pbSolver::setupNewSimObject(SIM_Object* object){
	std::cout << "\t" << object->getName() << " id: " << object->getObjectId() << std::endl;
	return true;
}
bool SIM_pbSolver::updateSimObject(SIM_Object* object){
	std::cout << "\t" << object->getName() << " id: " << object->getObjectId() << std::endl;
	
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
	
	GU_DetailHandleAutoWriteLock         gdl(geometry_copy->lockGeometry());
	GU_Detail           &gdp = *gdl.getGdp();
	GEO_Point           *geopt;
	UT_Vector3			dp(1,0,0);
	
	// Integrate our velocities.
	gdp.translate( dp, 0, 0, 1, 0, 0); 	
	//FOR_ALL_GPOINTS((&gdp), geopt)
	//{
	//	geopt->getPos() = geopt->getPos() + dp;
	//}
	
	// Store the integrated simulation state in geometry_copy
	geometry_copy->releaseGeometry();
	
	return true;
}
	
SIM_Solver::SIM_Result
SIM_pbSolver::solveObjectsSubclass ( SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep){
	SIM_ObjectArray		filtered;
	SIM_Object			*object, *new_object;
	UT_String			group, simfilename;
	OP_Node				*thisSolverNode;
	int					i;
	
	const SIM_DataFilterByType hard_filter 	= SIM_DataFilterByType("SIM_ConRelHard");
	const SIM_DataFilterByType pin_filter 	= SIM_DataFilterByType("SIM_ConAnchorObjPointPos");
	const SIM_DataFilterByType goal_filter 	= SIM_DataFilterByType("SIM_ConAnchorWorldSpacePos");
	
	const fpreal t_end = engine.getSimulationTime();
	const fpreal currTime = engine.getSimulationTime();
	
	// Evaluate all of the solver node parameters at this particular time
	thisSolverNode = getCreatorNode();
	thisSolverNode->evalString(simfilename, theSimeFileName.getToken(), 0, currTime );
	std::cout << "Preparing data for simulation time: " << currTime << std::endl; 
	
	// Loop through new objects.
	if (newobjects.entries() > 0) {
		std::cout << "Setting up new objects in sim:" << std::endl; 
		for( i = 0; i < newobjects.entries(); i++ ){	
			new_object = newobjects(i);
	    	setupNewSimObject(new_object);
		}	
	}

	// Loop through all the objects.
	if (objects.entries() > 0){
		std::cout << "Processing objects in sim:" << std::endl; 
		for( i = 0; i < objects.entries(); i++ ){ 
			object = objects(i);
	    	updateSimObject(object);  
		}	
	}
	
	// Run the simulation for the given time_step
	
	// Update all the objects
	
	//if( something went wrong )
	//   return SIM_Solver::SIM_SOLVER_FAIL;
		
	return SIM_Solver::SIM_SOLVER_SUCCESS;
}		

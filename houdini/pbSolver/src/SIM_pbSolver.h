#ifndef __SIM_pbSolver_h__
#define __SIM_pbSolver_h__

#include "libmain.h"

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <iostream.h>
#include <string.h>
#include <strstream.h>
#include <dlfcn.h>

#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <CH/CH_LocalVariable.h>
#include <GEO/GEO_AttributeHandle.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPart.h>
#include <GU/GU_PrimPoly.h>
#include <GU/GU_RayIntersect.h>
#include <GU/GU_PrimTriStrip.h>
#include <GU/GU_DetailHandle.h>
#include <OP/OP_Parameters.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <DOP/DOP_Node.h>
#include <PRM/PRM_Include.h>
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
#include <UT/UT_String.h>
#include <UT/UT_HashTable.h>
#include <UT/UT_Hash.h>
#include <UT/UT_IStream.h>
#include <UT/UT_VoxelArray.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Floor.h>
#include <UT/UT_Vector3.h>
#include <UT/UT_WorkBuffer.h>
#include <UT/UT_Interrupt.h>

physbam_simulation* (*hpi_create_simulation)();
bool (*hpi_destroy_simulation)(physbam_simulation*);
physbam_object* (*hpi_add_object)(physbam_simulation*, const data_exchange::simulation_object*);
physbam_force* (*hpi_add_force)(physbam_simulation*, const data_exchange::force*);
bool (*hpi_apply_force_to_object)(physbam_object*, physbam_force*);
bool (*hpi_simulate_frame)(physbam_simulation*);
int (*hpi_get_id)(physbam_base * obj, const char * attribute);
void (*hpi_set_int)(physbam_base * obj, int id, int x);
int (*hpi_get_int)(const physbam_base * obj, int id);
void (*hpi_set_float)(physbam_base * obj, int id, float x);
float (*hpi_get_float)(const physbam_base * obj, int id);
void (*hpi_get_vf3_array)(const physbam_base * obj, int id, data_exchange::vf3 * x, int length, int start);

class SIM_pbSolver : public SIM_Solver, public SIM_OptionsUser
{
private:
	static std::map<int, physbam_simulation*>	simulations;			// global store for all of the simulation instances
	std::map<int, physbam_object*>				objects;				// local simulation objects storage
	
public:
	GETSET_DATA_FUNCS_F("yourownaccuracy", MyOwnAccuracy);

protected:
    explicit				SIM_pbSolver(const SIM_DataFactory *factory);
    virtual					~SIM_pbSolver();
    
	// This implements our own solver step
	SIM_Solver::SIM_Result 	solveObjectsSubclass (SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep);
	
	bool					setupNewSimObject(physbam_simulation* sim, SIM_Object* object);
	bool					updateSimObject(physbam_simulation* sim, SIM_Object* object);		
		
private:
	static const SIM_DopDescription* getSolverDopDescription();
	
	DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_pbSolver, SIM_Solver, "PhysBAM Deformable Solver", getSolverDopDescription());	
	physbam_simulation*		pbSim(void);								// returns existing or creates new simulation
};

#endif

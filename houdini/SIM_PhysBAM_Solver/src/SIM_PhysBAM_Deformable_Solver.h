#ifndef __SIM_PhysBAM_Deformable_Solver_h__
#define __SIM_PhysBAM_Deformable_Solver_h__

#include "wrapper_header.h"
extern interface_routines ir;

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <iostream.h>
#include <string.h>
#include <dlfcn.h>

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
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_Geometry.h>
#include <SIMZ/SIM_SopGeometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Random.h>
#include <SIM/SIM_RandomTwister.h>
#include <SIM/SIM_Position.h>
#include <SIM/SIM_PRMShared.h>
#include <SIM/SIM_Guide.h>
#include <SIM/SIM_GuideShared.h>
#include <SIM/SIM_Solver.h>
#include <RBD/RBD_State.h>
#include <UT/UT_String.h>
#include <UT/UT_HashTable.h>
#include <UT/UT_Hash.h>
#include <UT/UT_IStream.h>
#include <UT/UT_VoxelArray.h>
#include <UT/UT_Floor.h>
#include <UT/UT_Vector3.h>
#include <UT/UT_WorkBuffer.h>
#include <UT/UT_Interrupt.h>

#include "SIM_PhysBAM_WorldData.h"

#include "logtools.h"

class SIM_PhysBAM_Deformable_Solver : public SIM_Solver, public SIM_OptionsUser
{	
public:
	//GETSET_DATA_FUNCS_F("yourownaccuracy", MyOwnAccuracy);

protected:
    explicit				SIM_PhysBAM_Deformable_Solver(const SIM_DataFactory *factory);
    virtual					~SIM_PhysBAM_Deformable_Solver();
    
	/// This implements our own solver step
	SIM_Solver::SIM_Result 	solveObjectsSubclass (SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep);
	
	bool					setupNewSimObject(physbam_simulation* sim, SIM_Object* object, SIM_Time time);
	bool					updateSimObject(physbam_simulation* sim, SIM_Object* object, const SIM_Time &timestep);		
		
private:
	SIM_PhysBAM_WorldData 	*worlddata;
	static const SIM_DopDescription* getSolverDopDescription();
	
	DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_PhysBAM_Deformable_Solver, SIM_Solver, "PhysBAM_Deformable_Solver", getSolverDopDescription());	
};

#endif

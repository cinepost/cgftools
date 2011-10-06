#ifndef __SIM_pbSolver_h__
#define __SIM_pbSolver_h__

#include <iostream.h>
#include <string.h>
#include <strstream.h>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/SimpleLayout.hh>
#include <UT/UT_String.h>
#include <UT/UT_HashTable.h>
#include <UT/UT_Hash.h>
#include <UT/UT_IStream.h>
#include <UT/UT_VoxelArray.h>
#include <GU/GU_DetailHandle.h>
#include <OP/OP_OperatorTable.h>
#include <DOP/DOP_Node.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_Solver.h>
#include <SIM/SIM_DataUtils.h>
#include <SIM/SIM_DataFactory.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_DataFilter.h>
#include <SIM/SIM_Relationship.h>
#include <SIM/SIM_RelationshipGroup.h>

class SIM_pbSolver : public SIM_Solver, public SIM_OptionsUser
{
public:
	GETSET_DATA_FUNCS_F("yourownaccuracy", MyOwnAccuracy);

protected:
    explicit				SIM_pbSolver(const SIM_DataFactory *factory);
    virtual					~SIM_pbSolver();
    
	// This implements our own solver step
	SIM_Solver::SIM_Result 	solveObjectsSubclass (SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep);
	
	bool					setupNewSimObject(SIM_Object* object);
	bool					updateSimObject(SIM_Object* object);		
		
private:
	static const SIM_DopDescription* getSolverDopDescription();
	
	DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_pbSolver, SIM_Solver, "PhysBAM Deformable Solver", getSolverDopDescription());
};

#endif

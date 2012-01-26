#ifndef __SIM_pbWorldData_h__
#define __SIM_pbWorldData_h__

#include "SIM_PhysBAM_Commons.h"
#include "HPI_Solid_Object.h"
#include "HPI_Baked_Object.h"
#include "HPI_Ground_Object.h"

#include <iostream>
#include <map>

#include <UT/UT_Floor.h>
#include <UT/UT_Vector3.h>
#include <UT/UT_WorkBuffer.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPart.h>
#include <GU/GU_PrimPoly.h>
#include <GU/GU_RayIntersect.h>
#include <GU/GU_PrimTriStrip.h>
#include <GEO/GEO_AttributeHandle.h>
#include <PRM/PRM_Include.h>
#include <SIM/SIM_Data.h>
#include <SIM/SIM_DataFactory.h>
#include <SIM/SIM_Engine.h>
#include <SIM/SIM_Options.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Random.h>
#include <SIM/SIM_RandomTwister.h>
#include <SIM/SIM_Position.h>
#include <SIM/SIM_PRMShared.h>
#include <SIM/SIM_Guide.h>
#include <SIM/SIM_GuideShared.h>
#include <SIM/SIM_Geometry.h>
#include <SIMZ/SIM_SopGeometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_ForceGravity.h>
#include <SIM/SIM_EmptyData.h>
#include <SIM/SIM_Constraint.h>
#include <SIM/SIM_ConstraintIterator.h>
#include <SIM/SIM_DataFilter.h>
#include <SIM/SIM_ConAnchorSpatial.h>
#include <SIM/SIM_ConAnchorRotational.h>
#include <SIM/SIM_ConRel.h>
#include <SIM/SIM_ScalarField.h>
#include <SIM/SIM_VectorField.h>
#include <SIMZ/SIM_SopScalarField.h>
#include <SIMZ/SIM_SopVectorField.h>
#include <SIM/SIM_SDF.h>
#include <SIM/SIM_PhysicalParms.h>

class SIM_PhysBAM_WorldData : public SIM_Data
{
public:
	std::map<int, physbam_force*>		*getForces();
	std::map<int, HPI_Object*>			*getObjects();
	physbam_force						*getForce(int id);
	HPI_Object							*getSolidObject(int id);
	bool								objectExists(int id);
	bool								forceExists(int id);
	bool								fluidObjectExists(int id);
	
	physbam_force						*addNewForce(const SIM_Data *force);
	physbam_object						*addNewObject(SIM_Object *object, SIM_Time time);
	physbam_object						*addNewSolidObject(SIM_Object *object);
	physbam_object						*addNewBakedObject(SIM_Object *object);	
	physbam_object						*addNewGroundObject(SIM_Object *object);
	
	physbam_simulation					*getSimulation();

protected:
	explicit 	SIM_PhysBAM_WorldData(const SIM_DataFactory *factory);
	virtual 	~SIM_PhysBAM_WorldData();

	// This ensures that this data is always kept in RAM, even when the cache runs out of space and writes out the simulation step to disk. This is necessary because the ODE data can't be written to disk.
	virtual bool getCanBeSavedToDiskSubclass() const;

	// Start from scratch. We create a brand new world.
	virtual void initializeSubclass();

	// To make one world equal to another, copy the data exactly. The share count lets several of these data share the same PhysBAM world and bodies without worrying that the ODE data will get deleted as long as any SIM_PhysBAM_WorldData is holding onto it.
	virtual void makeEqualSubclass(const SIM_Data *src);

	virtual void handleModificationSubclass (int code);

private:
	static const SIM_DopDescription *getDopDescription();
	DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_PhysBAM_WorldData, SIM_Data, "PhysBAM_World", getEmptyDopDescription());
	
	void	clear();

public:
	physbam_simulation					*simulation;
	std::map<int, physbam_force*>		*forces;
	std::map<int, HPI_Object*>			*objects;
	int									*m_shareCount;
};

#endif

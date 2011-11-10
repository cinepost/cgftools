#ifndef __SIM_PhysBAM_Body_h__
#define __SIM_PhysBAM_Body_h__

#include <UT/UT_HashTable.h>
#include <UT/UT_Hash.h>
#include <UT/UT_IStream.h>
#include <UT/UT_VoxelArray.h>
#include <GU/GU_DetailHandle.h>
#include <GEO/GEO_Point.h>
#include <GEO/GEO_AttributeOwner.h>
#include <SIM/SIM_Data.h>
#include <SIM/SIM_DataFactory.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_Names.h>

class SIM_PhysBAM_Body : public SIM_Data,
			public SIM_OptionsUser
{
public:
	GET_DATA_FUNC_B("deformable", Deformable);
	GET_DATA_FUNC_F("stiffness", Stiffness);
    GET_DATA_FUNC_F("damping", Damping);
    GET_DATA_FUNC_I("approx_number_of_cells", CellsApprox);	

protected:
	explicit 	SIM_PhysBAM_Body(const SIM_DataFactory *factory);
	virtual 	~SIM_PhysBAM_Body();
	virtual void 	initializeSubclass ();

};

#endif

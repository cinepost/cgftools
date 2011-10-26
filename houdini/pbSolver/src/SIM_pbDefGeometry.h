#ifndef __SIM_pbDefGeometry_h__
#define __SIM_pbDefGeometry_h__

#include "HPI_trimesh.h"
#include "SIM_pbGeometry.h"

#include <iostream>

#include <GU/GU_Detail.h>
#include <SIM/SIM_Data.h>
#include <SIM/SIM_OptionsUser.h>

#define NAME_STIFFNESS	"stiffness"
#define NAME_DAMPING	"damping"

class SIM_pbDefGeometry : 	public SIM_Data,
							public SIM_OptionsUser

{
public:
	GETSET_DATA_FUNCS_F(NAME_STIFFNESS, Stiffness);  	
	GETSET_DATA_FUNCS_F(NAME_DAMPING, Damping);  	

protected:
	explicit		SIM_pbDefGeometry(const SIM_DataFactory *factory);
    virtual			~SIM_pbDefGeometry();																			
											
private:
	float 	stiffness;
	float 	damping;
	static const SIM_DopDescription *getDopDescription();

	DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_pbDefGeometry, SIM_Data, "PhysBAM_Deformable", getDopDescription());
};

#endif

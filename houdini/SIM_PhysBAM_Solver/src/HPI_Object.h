#ifndef __HPI_Object_h__
#define __HPI_Object_h__

#include "SIM_PhysBAM_Commons.h"
#include <SIM/SIM_Object.h>
#include <SIM/SIM_Geometry.h>

class HPI_Object{
	public:
		HPI_Object();
		~HPI_Object();
	
		physbam_object*	getPhysbamObject();
		int				getUid();
		virtual	bool	setFromObject(SIM_Object *object, physbam_simulation *sim){ return true; };
		virtual bool	updateSimulatedObject(SIM_Object *object, physbam_simulation *sim, const SIM_Time &timestep){ return true; };	
			
	protected:
		physbam_object*	pb_object;
		int				uid;	

};

#endif

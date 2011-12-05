#ifndef __HPI_Object_h__
#define __HPI_Object_h__

#include "SIM_PhysBAM_Commons.h"

class HPI_Object{
	public:
		HPI_Object();
		~HPI_Object();
	
		physbam_object*	getPhysbamObject();
		int				getUid();
		
	protected:
		physbam_object*	pb_object;
		int				uid;	

};

#endif

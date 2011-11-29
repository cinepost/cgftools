#ifndef __HPI_Fluid_Object_h__
#define __HPI_Fluid_Object_h__

#include "SIM_PhysBAM_Commons.h"

class HPI_Fluid_Object{
	public:
		HPI_Fluid_Object();
		HPI_Fluid_Object(float s_x, float s_y, float s_z, unsigned int d_x, unsigned int d_y, unsigned int d_z);
		~HPI_Fluid_Object();
	
		void			setSize(float s_x, float s_y, float s_z);
		void			setDivisions(unsigned int d_x, unsigned int d_y, unsigned int d_z);
		void			setPhysbamObject(physbam_object *obj);
		void			setFluidType(unsigned char type);
		unsigned char	getFluidType();
		physbam_object*	getPhysbamObject();
		
		float SIZE_X();
		float SIZE_Y();
		float SIZE_Z();
		
		unsigned int DIV_X();
		unsigned int DIV_Y();
		unsigned int DIV_Z();	
		
	private:
		physbam_object*	object;
		float 			size_x, size_y, size_z;
		unsigned int 	div_x, div_y, div_z;
		unsigned char 	fluid_type;
};

#endif

#include "HPI_Fluid_Object.h"

#include "logtools.h"

HPI_Fluid_Object::HPI_Fluid_Object() : size_x(0), size_y(0), size_z(0), object(0), fluid_type(0){
	
}

HPI_Fluid_Object::HPI_Fluid_Object(float s_x, float s_y, float s_z, unsigned int d_x, unsigned int d_y, unsigned int d_z) : object(0), fluid_type(0){
	setSize(s_x, s_y, s_z);
	setDivisions(d_x, d_y, d_z);
}

void
HPI_Fluid_Object::setSize(float s_x, float s_y, float s_z){
	size_x = s_x;
	size_y = s_y;
	size_z = s_z;
	LOG_INDENT;
	LOG("HPI_Fluid_Object set size to: " << size_x << " " << size_y << " " << size_z);
	LOG_UNDENT; 
}

void
HPI_Fluid_Object::setDivisions(unsigned int d_x, unsigned int d_y, unsigned int d_z){
	div_x = d_x;
	div_y = d_y;
	div_z = d_z;
	LOG_INDENT;
	LOG("HPI_Fluid_Object set divisions to: " << div_x << " " << div_y << " " << div_z);
	LOG_UNDENT;	
}

float
HPI_Fluid_Object::SIZE_X(){
	return size_x;
}

float
HPI_Fluid_Object::SIZE_Y(){
	return size_y;	
}

float
HPI_Fluid_Object::SIZE_Z(){
	return size_z;
}

unsigned int
HPI_Fluid_Object::DIV_X(){
	return div_x;
}

unsigned int
HPI_Fluid_Object::DIV_Y(){
	return div_y;	
}

unsigned int
HPI_Fluid_Object::DIV_Z(){
	return div_z;
}

void
HPI_Fluid_Object::setPhysbamObject(physbam_object*	obj){
	object = obj;
}

physbam_object*
HPI_Fluid_Object::getPhysbamObject(){
	return object;
}

void
HPI_Fluid_Object::setFluidType(unsigned char type){
	fluid_type = type;
}

unsigned char
HPI_Fluid_Object::getFluidType(){
	return fluid_type;
}

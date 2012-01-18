#include "HPI_Baked_Object.h"
#include "logtools.h"

extern interface_routines ir;

HPI_Trimesh*
HPI_Baked_Object::getTrimesh(){
	return trimesh;
}

HPI_Baked_Object::HPI_Baked_Object() : HPI_Object() {
	trimesh = new HPI_Trimesh();	
}

HPI_Baked_Object::~HPI_Baked_Object() {
	delete trimesh;
}

bool
HPI_Baked_Object::setFromObject(SIM_Object *object, physbam_simulation *sim){
	LOG("HPI_Baked_Object::setFromObject(SIM_Object *object, physbam_simulation *sim) called.");
	uid = object->getObjectId();
	
	LOG("Setting trimesh from SIM_Object...");
	trimesh->setFromObject(object);
	LOG("Done.");
	
	LOG("Adding scripted body to simulation...");
	pb_object = ir.call<physbam_object*>("add_scripted_geometry", sim, *trimesh->getMesh(), *trimesh->getPositions());
	LOG("Done.");
	
	if(!pb_object)
		return false;    
					
	return true;	
}

bool
HPI_Baked_Object::updateSimulatedObject(SIM_Object *object, physbam_simulation *sim, const SIM_Time &timestep){
	return true;
}

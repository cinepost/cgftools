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

void
HPI_Baked_Object::appendKeyframe(SIM_Object *object, physbam_simulation *sim, const SIM_Time &time){
	const SIM_Position *sim_pos = object->getPosition();
	UT_Quaternion	orient;
	UT_Vector3		pos;
	
	sim_pos->getOrientation(orient);
	sim_pos->getPosition(pos);
	
	ir.call<void>("add_scripted_body_keyframe", pb_object, time, rigid_body_frame(vf3(pos.x(),pos.y(),pos.z()), vf4(orient.x(),orient.y(),orient.z(),orient.w())));
}	

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
	
	float friction = 0.5f;
	SIM_PhysicalParms	*scripted_object = SIM_DATA_CAST(object->getNamedSubData("SIM_PhysicalParms"), SIM_PhysicalParms);
	if(scripted_object){
		friction = scripted_object->getProperty( SIM_PROPERTY_FRICTION);
		LOG("Friction: " << friction);
	}else{
		LOG("Using default friction: " << friction);
	}
	

	LOG("Adding scripted body to simulation...");
	pb_object = ir.call<physbam_object*>("add_scripted_geometry", sim, *trimesh->getMesh(), *trimesh->getPositions());
	ir.call<void>("set_rigid_body_friction", pb_object, friction);	
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
HPI_Baked_Object::appendKeyframe(const SIM_Object *object, physbam_simulation *sim, const SIM_Time &time){
	const SIM_Position *sim_pos = object->getPosition();
	UT_Quaternion	orient;
	UT_Vector3		pos;
	
	sim_pos->getOrientation(orient);
	sim_pos->getPosition(pos);
	
	LOG("HPI_Baked_Object::appendKeyframe() appending keyframe for object: " << object->getName() << " at: " << (float)time);	
	
	//ir.call<void>("add_scripted_body_keyframe", pb_object, time, rigid_body_frame(vf3(pos.x(),pos.y(),pos.z()), vf4(orient.x(),orient.y(),orient.z(),orient.w())));
	ir.call<void>("add_scripted_body_keyframe", pb_object, (float)time, rigid_body_frame(vf3(pos.x(),pos.y(),pos.z()), vf4(-orient.z(),orient.y(),orient.x(),orient.w())));
}	

#include "HPI_Ground_Object.h"
#include "logtools.h"

extern interface_routines ir;

HPI_Ground_Object::HPI_Ground_Object() : HPI_Object() {
	
}

HPI_Ground_Object::~HPI_Ground_Object() {

}

bool
HPI_Ground_Object::setFromObject(SIM_Object *object, physbam_simulation *sim){
	LOG_INDENT;
	LOG("HPI_Ground_Object::setFromObject(SIM_Object *object, physbam_simulation *sim) called.");
	uid = object->getObjectId();
	
	const SIM_Position *sim_pos = object->getPosition();
	UT_Vector3 		t, n;
	UT_DMatrix4		xform;
	UT_Matrix3		rot;
	
	sim_pos->getTransform(xform);
	
	xform.extractRotate(rot);	
	xform.getTranslates(t);
	
	n = UT_Vector3 (0.0f ,1.0f ,0.0f);
	
	//LOG("N: " << n.x() << " " << n.y() << " " << n.z());
	n *= rot;
	//LOG("N: " << n.x() << " " << n.y() << " " << n.z());
	//LOG("T: " << t.x() << " " << t.y() << " " << t.z());
	
	float friction = 0.5f;
	SIM_PhysicalParms	*ground_object = SIM_DATA_CAST(object->getNamedSubData("SIM_PhysicalParms"), SIM_PhysicalParms);
	if(ground_object){
		friction = ground_object->getProperty( SIM_PROPERTY_FRICTION);
		LOG("Friction: " << friction);
	}else{
		LOG("Using default friction: " << friction);
	}
	
	pb_object = ir.call<physbam_object*>("add_ground", sim, vf3(t.x(), t.y(), t.z()), vf3(n.x(), n.y(), n.z()));
	ir.call<void>("set_rigid_body_friction", pb_object, friction);
	
	if(!pb_object)
		LOG("Done");
		LOG_UNDENT;
		return false;    
	
	LOG("Done");
	LOG_UNDENT;				
	return true;	
}

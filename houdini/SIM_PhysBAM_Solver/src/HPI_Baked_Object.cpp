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
	
	const SIM_Position *sim_pos = object->getPosition();
	UT_Vector3 		t, n;
	UT_DMatrix4		xform;
	UT_Matrix3		rot;
	
	sim_pos->getTransform(xform);
	
	xform.extractRotate(rot);	
	xform.getTranslates(t);
	
	n = UT_Vector3 (0.0f ,1.0f ,0.0f);
	
	LOG("N: " << n.x() << " " << n.y() << " " << n.z());
	n *= rot;
	LOG("N: " << n.x() << " " << n.y() << " " << n.z());
	LOG("T: " << t.x() << " " << t.y() << " " << t.z());	
	
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
HPI_Baked_Object::updateSimulatedObject(SIM_Object *object, physbam_simulation *sim){
	return true;
}

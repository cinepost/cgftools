#include "HPI_Solid_Object.h"
#include "logtools.h"

extern interface_routines ir;

HPI_Solid_Object::HPI_Solid_Object() : HPI_Object() {
	trimesh = new HPI_Trimesh();
}

HPI_Solid_Object::~HPI_Solid_Object() {
	delete trimesh;
}

HPI_Trimesh*
HPI_Solid_Object::getTrimesh(){
	return trimesh;
}

bool
HPI_Solid_Object::setFromObject(SIM_Object *object, physbam_simulation *sim){
	LOG("HPI_Solid_Object::setFromObject(SIM_Object *object, physbam_simulation *sim) called.");
	uid = object->getObjectId();
   
	SIM_EmptyData	*body_data = NULL;
	body_data = SIM_DATA_CAST(object->getNamedSubData("PhysBAM_Body"), SIM_EmptyData);
	if(body_data == NULL){
		LOG("HPI_Solid_Object::setFromObject(SIM_Object *object, physbam_simulation *sim) no PhysBAM_Body data found on object:" << uid);
		return false;
	}
	
	float 		poisson				= 0.45f;
	float		mass 				= 10.0;
	float 		stiffness 			= 1e3;
	float 		damping 			= .01f;
	int 		approx_number_cells = 100;
	UT_Vector3	velocity 			= UT_Vector3(0.0f,0.0f,0.0f);
	UT_Vector3	angular_velocity 	= UT_Vector3(0.0f,0.0f,0.0f);
	UT_Vector3	angular_center 		= UT_Vector3(0.0f,0.0f,0.0f);
			
	LOG("Fetching options from PhysBAM_Body object data.");
	const SIM_Options *data = &body_data->getData();
	if(data){		
		if(data->hasOption("poisson"))poisson = data->getOptionF("poisson");
		if(data->hasOption("mass"))mass = data->getOptionF("mass");
		if(data->hasOption("stiffness"))stiffness = data->getOptionF("stiffness");
		if(data->hasOption("damping"))damping = data->getOptionF("damping");
		if(data->hasOption("approx_number_of_cells"))approx_number_cells = data->getOptionI("approx_number_of_cells");
		if(data->hasOption("velocity"))velocity = data->getOptionV3("velocity");
		if(data->hasOption("angular_velocity"))angular_velocity = data->getOptionV3("angular_velocity");
		if(data->hasOption("angular_center"))angular_center = data->getOptionV3("angular_center");	
	}
	
	LOG("Setting trimesh from SIM_Object...");
	trimesh->setFromObject(object);
	LOG("Done.");
	
	LOG("Adding deformable body to simulation...");
	pb_object = ir.call<physbam_object*>("add_deformable_body", sim, *trimesh->getMesh(), *trimesh->getPositions(), mass , approx_number_cells);
	ir.call<void>("set_deformable_object_rigid_motion", pb_object, 
		vf3(velocity.x(),velocity.y(),velocity.z()), 
		vf3(angular_velocity.x(),angular_velocity.y(),angular_velocity.z()),
		vf3(angular_center.x(),angular_center.y(),angular_center.z()));
	LOG("Done.");
	
	if(!pb_object)
		return false;
			
	/// Create volume force for deformable body
	physbam_force	*f1 = ir.call<physbam_force*>("add_volumetric_force", sim, stiffness, poisson, damping);
	ir.call<void>("apply_force_to_object", pb_object, f1);	
			
	/// Now create constraints
	/*
	SIM_ConstraintIterator::initConstraints(*object, time);
	SIM_DataFilterByType 	anchorFilter("SIM_ConAnchor");
	SIM_ConstraintIterator 	conit(*object, 0, &anchorFilter, &anchorFilter, time);

	for (conit.rewind(); !conit.atEnd(); conit.advance())
	{
		SIM_ConRel                      *conrel;
		const SIM_ConAnchorSpatial      *spanchor, *goalspanchor;
		const SIM_ConAnchorRotational  	*rotanchor, *goalrotanchor;
		
		conrel = conit.getConRel();
		spanchor = SIM_DATA_CASTCONST(conit.getCurrentAnchor(), SIM_ConAnchorSpatial);
		goalspanchor = SIM_DATA_CASTCONST(conit.getGoalAnchor(), SIM_ConAnchorSpatial);
		if( spanchor && goalspanchor )
		{
			// Handle spatial constraints.
			LOG("Constraint spatial." << std::endl);
		}
		
		rotanchor = SIM_DATA_CASTCONST(conit.getCurrentAnchor(), SIM_ConAnchorRotational);
		goalrotanchor = SIM_DATA_CASTCONST(conit.getGoalAnchor(), SIM_ConAnchorRotational);
		if( rotanchor && goalrotanchor )
		{
			// Handle Rotational constraints.
			LOG("Constraint rotational." << std::endl);
		}
	}

	//SIM_DataFilterByType 	anchorFilter("SIM_Constraint");
	//SIM_ConstraintIterator 	conit(*object, 0, &anchorFilter, &anchorFilter, time);
	//const SIM_Constraint	*constraint = 0;
	
	//for (conit.rewind(); !conit.atEnd(); conit.advance())
	//{
	//	constraint = conit.getConstraint();
	//	LOG("Constraint: " << constraint->getDataType() << std::endl);
	//}
	*/                
					
	return true;
}

bool
HPI_Solid_Object::updateSimulatedObject(SIM_Object *object, physbam_simulation *sim){	
	if(pb_object){
		int len = ir.call<int>("size_position", pb_object);
		std::vector<vf3> pos_array(len);
        raw_array<vf3> ra(pos_array);
        ir.call<void>("get_position", pb_object, &ra);			
		trimesh->setToObject(object, &pos_array);
		return true;
	}
	return false;
}

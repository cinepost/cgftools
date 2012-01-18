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
HPI_Solid_Object::updateSimulatedObject(SIM_Object *object, physbam_simulation *sim, const SIM_Time &timestep){	
	if(pb_object){
		
		SIM_GeometryCopy	*embed_mesh_data = SIM_DATA_CAST(object->getNamedSubData("Embed_Mesh"), SIM_GeometryCopy);
		if(embed_mesh_data){
			GU_DetailHandleAutoWriteLock	gdl(embed_mesh_data->lockGeometry());
			GU_Detail *gdp = gdl.getGdp();
			gdp->clearAndDestroy ();
			
			/// retrieve embed mesh from physbam
			int len = ir.call<int>("size_embed_position", pb_object);
			GEO_Point* added_points[len];
			std::vector<vf3> x_array(len);
			raw_array<vf3> rp(x_array);
			ir.call<void>("get_embed_position", pb_object, &rp);
			for(size_t i=0; i<x_array.size(); i++){
				added_points[i] = gdp->appendPoint();
				added_points[i]->getPos().assign(x_array[i].data[0], x_array[i].data[1], x_array[i].data[2], 1.0);
				
				//printf("(%g %g %g)\n", x_array[i].data[0], x_array[i].data[1], x_array[i].data[2]);
			}
		
			
			int mesh_len=ir.call<int>("size_embed_mesh", pb_object);
			std::vector<vi4> embedding_mesh(mesh_len);
			raw_array<vi4> rb(embedding_mesh);
			ir.call<void>("get_embed_mesh", pb_object, &rb);
			
			GU_PrimTriStrip     *tristrip;
			/// add a color attribute
			GB_AttributeRef attCd = gdp->addDiffuseAttribute(GEO_PRIMITIVE_DICT);
			UT_Vector3 pos_color(1, 0, 0);
			UT_Vector3 vel_color(0, 1, 0);
			//if( options.hasOption(theGuideColorName.getToken()) )
			//	color = options.getOptionV3(theGuideColorName.getToken());
            			 
			for(size_t i=0; i<embedding_mesh.size(); i++){
				tristrip =  GU_PrimTriStrip::build(gdp, 6, 0);
				
				tristrip->setValue<UT_Vector3>(attCd, pos_color);
				
				(*tristrip)(0).setPt(added_points[embedding_mesh[i].data[3]-1]);
				(*tristrip)(1).setPt(added_points[embedding_mesh[i].data[1]-1]);
				(*tristrip)(2).setPt(added_points[embedding_mesh[i].data[0]-1]);
				(*tristrip)(3).setPt(added_points[embedding_mesh[i].data[2]-1]);
				(*tristrip)(4).setPt(added_points[embedding_mesh[i].data[3]-1]);
				(*tristrip)(5).setPt(added_points[embedding_mesh[i].data[1]-1]);
				//printf("(%i %i %i %i)...", embedding_mesh[i].data[0], embedding_mesh[i].data[1], embedding_mesh[i].data[2], embedding_mesh[i].data[3]);
			}
			//printf("Done.\n");
		
			int vlen = ir.call<int>("size_embed_velocity", pb_object);
			std::vector<vf3> v_array(vlen);
			raw_array<vf3> rv(v_array);
			ir.call<void>("get_embed_velocity", pb_object, &rv);
			GEO_Point* pt1;
			GEO_Point* pt2;
			GU_PrimPoly *poly;
			
			for(size_t i=0; i<v_array.size(); i++){
				pt1 = added_points[i];
				pt2 = gdp->appendPoint();
				pt2->setPos(pt1->getPos() + UT_Vector4(v_array[i].data[0], v_array[i].data[1], v_array[i].data[2], 1) * timestep);
				poly = (GU_PrimPoly *)gdp->appendPrimitive(GEOPRIMPOLY);
				poly->setValue<UT_Vector3>(attCd, vel_color);
				poly->appendVertex(pt1);
				poly->appendVertex(pt2);
				//printf("(%g %g %g)\n", v_array[i].data[0], v_array[i].data[1], v_array[i].data[2]);		
			
			}
		}
		
		/// retrieve point positions from physbam
		int len = ir.call<int>("size_position", pb_object);
		std::vector<vf3> pos_array(len);
        raw_array<vf3> ra(pos_array);
        ir.call<void>("get_position", pb_object, &ra);			
		trimesh->setToObject(object, &pos_array);
		return true;
	}
	return false;
}

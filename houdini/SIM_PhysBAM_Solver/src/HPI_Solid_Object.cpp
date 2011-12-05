#include "HPI_Solid_Object.h"
#include "logtools.h"

extern interface_routines ir;

HPI_Solid_Object::HPI_Solid_Object() : pb_object(0), trimesh(0), uid(0) {
	
}

physbam_object*
HPI_Solid_Object::getPhysbamObject(){
	return pb_object;
}

HPI_TriMesh*
HPI_Solid_Object::getTrimesh(){
	return trimesh;
}

int
HPI_Solid_Object::getUid(){
	return uid;
}

bool
HPI_Solid_Object::setFromObject(SIM_Object *object, physbam_simulation *sim){
	LOG("HPI_Solid_Object::setFromObject(SIM_Object *object, physbam_simulation *sim) called.");
	SIM_SopGeometry* geometry;
	geometry = SIM_DATA_CAST(object->getNamedSubData("Geometry"), SIM_SopGeometry);
	uid = object->getObjectId();
	
    if(!geometry)
		return false;
   
	SIM_EmptyData	*body_data = SIM_DATA_CAST(object->getNamedSubData("PhysBAM_Body"), SIM_EmptyData);
	
	float		mass 				= 10.0;
	float 		stiffness 			= 1e3;
	float 		damping 			= .01;
	int 		approx_number_cells = 100;
	UT_Vector3	velocity 			= UT_Vector3(0.0f,0.0f,0.0f);
	UT_Vector3	angular_velocity 	= UT_Vector3(0.0f,0.0f,0.0f);
	UT_Vector3	angular_center 		= UT_Vector3(0.0f,0.0f,0.0f);
			
	bool	deformable			= true;
	
	if(body_data){
		LOG("Fetching options from PhysBAM_Body object data.");
		const SIM_Options *data = &body_data->getData();
		
		if(data->hasOption("mass"))mass = data->getOptionF("mass");
		if(data->hasOption("stiffness"))stiffness = data->getOptionF("stiffness");
		if(data->hasOption("damping"))damping = data->getOptionF("damping");
		if(data->hasOption("approx_number_of_cells"))approx_number_cells = data->getOptionI("approx_number_of_cells");
		if(data->hasOption("velocity"))velocity = data->getOptionV3("velocity");
		if(data->hasOption("angular_velocity"))angular_velocity = data->getOptionV3("angular_velocity");
		if(data->hasOption("angular_center"))angular_center = data->getOptionV3("angular_center");	
	}
	
	const GU_Detail*				const gdp(geometry->getGeometry().readLock());
	const UT_Vector4F*				pt_pos;
	const GEO_Primitive*			prim;
	data_exchange::deformable_body 	db;
	
	int	pb_vertex_index = 0;
	trimesh = new HPI_TriMesh();
	
	FOR_ALL_PRIMITIVES(gdp, prim)
	{
		if(prim->getPrimitiveId() == GEOPRIMPOLY){
			const GEO_Vertex*	v;
			GEO_Point*	p;
			pair<std::map<GEO_Point*, int>::iterator,bool> ret;
			int i1, i2, i3, i4;
			switch(prim->getVertexCount()){
				case 3:
					v = &prim->getVertex(0);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i1 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i1 = pb_vertex_index++;
					}	
					
					v = &prim->getVertex(1);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i2 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i2 = pb_vertex_index++;
					}

					v = &prim->getVertex(2);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i3 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i3 = pb_vertex_index++;
					}
					
					//std::cout << "indexes3:" << i1 << " " << i2 << " " << i3 << std::endl;
					db.mesh.insert_polygon(data_exchange::vi3(i3, i2, i1));
					break;
				case 4:
					v = &prim->getVertex(0);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i1 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i1 = pb_vertex_index++;
					}	
					
					v = &prim->getVertex(1);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i2 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i2 = pb_vertex_index++;
					}

					v = &prim->getVertex(2);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i3 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i3 = pb_vertex_index++;
					}

					v = &prim->getVertex(3);
					p = v->getPt();
					ret = trimesh->points.insert ( std::pair<GEO_Point*, int>(p, pb_vertex_index) );
					if(ret.second==false){
						// this point already existed. use existing index
						i4 = ret.first->second;
					}else{
						// new point. we need to store in in def body
						db.position.push_back(data_exchange::vf3(p->getPos()[0], p->getPos()[1], p->getPos()[2]));
						i4 = pb_vertex_index++;
					}					
					//std::cout << "indexes4:" << i1 << " " << i2 << " " << i3 << " " << i4 << std::endl;
					db.mesh.insert_polygon(data_exchange::vi4(i4, i3, i2, i1));				
					break;
				default:
					std::cout << prim->getVertexCount();
					break;
			}
		}
	}

	db.mass 				= mass;
	db.approx_number_cells 	= approx_number_cells;
	db.velocity 			= data_exchange::vf3(velocity.x(),velocity.y(),velocity.z());
	db.angular_velocity 	= data_exchange::vf3(angular_velocity.x(),angular_velocity.y(),angular_velocity.z());
	db.angular_center 		= data_exchange::vf3(angular_center.x(),angular_center.y(),angular_center.z());
	
	pb_object = ir.add_object(sim, &db);
	
	if(!pb_object)
		return false;
			
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
					
	/// Create volume force for deformable body
	data_exchange::volumetric_force vf;
	vf.stiffness = stiffness;
	vf.damping = damping;
	physbam_force* f1 = ir.add_force(sim, &vf);
	ir.apply_force_to_object(pb_object, f1);
	
	return true;
}

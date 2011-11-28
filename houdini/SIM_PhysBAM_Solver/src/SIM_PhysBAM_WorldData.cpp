#include "SIM_PhysBAM_WorldData.h"

extern interface_routines ir;

#include <SIM/SIM_Data.h>
#include <SIM/SIM_DataFactory.h>
#include "logtools.h"

SIM_PhysBAM_WorldData::SIM_PhysBAM_WorldData(const SIM_DataFactory *factory) : SIM_Data(factory),
	simulation(NULL),
	objects(0),
	forces(0),
	trimeshes(0),
	m_shareCount(0){

};

SIM_PhysBAM_WorldData::~SIM_PhysBAM_WorldData(){ 
	clear();
};

std::map<int, HPI_TriMesh*>*
SIM_PhysBAM_WorldData::getTriMeshes(){
	return trimeshes;
}

HPI_TriMesh*
SIM_PhysBAM_WorldData::getTrimesh(int id){
	return trimeshes->find(id)->second;
}

std::map<int, physbam_object*>* 
SIM_PhysBAM_WorldData::getObjects(void){
	return objects;
};

physbam_force*
SIM_PhysBAM_WorldData::getForce(int id){
	return forces->find(id)->second;
}

std::map<int, physbam_force*>*
SIM_PhysBAM_WorldData::getForces(void){
	return forces;
}

bool
SIM_PhysBAM_WorldData::objectExists(int id){
	return objects->find( id ) != objects->end();
}

bool
SIM_PhysBAM_WorldData::forceExists(int id){
	return forces->find( id ) != forces->end();
}

bool
SIM_PhysBAM_WorldData::trimeshExists(int id){
	return trimeshes->find( id ) != trimeshes->end();
}

physbam_simulation* 
SIM_PhysBAM_WorldData::getSimulation(void){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::getSimulation() called.");
	if(!simulation){
		LOG("SIM_PhysBAM_WorldData::getSimulation() creating new simulation instace.");
		simulation		= ir.create_simulation("solids");
		LOG("SIM_PhysBAM_WorldData::getSimulation() created instance: "  << simulation);
		ir.set_string(simulation, ir.get_id(simulation, "output_directory"), "/opt/houdini/output/");
		ir.set_string(simulation, ir.get_id(simulation, "data_directory"), "/opt/PhysBAM-2011/Public_Data");
	}else{
		LOG("SIM_PhysBAM_WorldData::getSimulation() using existing simulation instance: " << simulation);
	}
	LOG("Done");
	LOG_UNDENT; 	
	return simulation;
};

bool SIM_PhysBAM_WorldData::getCanBeSavedToDiskSubclass() const{
	return false;
};

const SIM_DopDescription *
SIM_PhysBAM_WorldData::getDopDescription()
{
    static SIM_DopDescription	 theDopDescription(false,
						   "physbam_worlddata",
						   "PhysBAM Worlddata",
						   "PhysBAM_World",
						   classname(),
						   0);					   

    return &theDopDescription;
}

void SIM_PhysBAM_WorldData::initializeSubclass(){
	//LOG_INDENT;
	//LOG("SIM_PhysBAM_WorldData::initializeSubclass() called.");
	clear();
	simulation		= NULL;
	objects			= new std::map<int, physbam_object*>();
	forces			= new std::map<int, physbam_force*>();
	trimeshes		= new std::map<int, HPI_TriMesh*>();
	m_shareCount	= new int(1);
	//LOG("Done");
	//LOG_UNDENT;
};

void SIM_PhysBAM_WorldData::makeEqualSubclass(const SIM_Data *src){
	//LOG_INDENT;
	//LOG("SIM_PhysBAM_WorldData::makeEqualSubclass() called.");
	const SIM_PhysBAM_WorldData *world;
	world = SIM_DATA_CASTCONST(src, SIM_PhysBAM_WorldData);
	if( world ){
		clear();
		simulation		= world->simulation;	
		objects 		= world->objects;	
		forces 			= world->forces;
		trimeshes 		= world->trimeshes;						
		m_shareCount 	= world->m_shareCount;
		(*m_shareCount)++;
	}
	LOG("WorldData share count: " << (*m_shareCount));
	//LOG("Done");
	//LOG_UNDENT;	
}

void SIM_PhysBAM_WorldData::clear(){
	//LOG_INDENT;
	//LOG("SIM_PhysBAM_WorldData::clear() called.");
	if( m_shareCount ){
		(*m_shareCount)--;
		if(*m_shareCount == 0){
			if(simulation){
				LOG("SIM_PhysBAM_WorldData::clear() destroying sim: " << simulation);
				ir.destroy_simulation(simulation);
			}
			for ( typename std::map<int, physbam_object*>::iterator it = objects->begin(); it != objects->end(); ++it ) {
				delete it->second;
			}
			objects->clear();
			
			for ( typename std::map<int, physbam_force*>::iterator it = forces->begin(); it != forces->end(); ++it ) {
				delete it->second;
			}
			forces->clear();

			for ( typename std::map<int, HPI_TriMesh*>::iterator it = trimeshes->begin(); it != trimeshes->end(); ++it ) {
				delete it->second;
			}
			trimeshes->clear();

			delete 	m_shareCount;
		}
	}
	simulation		= NULL;
	objects			= NULL;
	forces			= NULL;
	trimeshes		= NULL;							
	m_shareCount 	= 0;
	//LOG("Done");
	//LOG_UNDENT;	
}

void 	
SIM_PhysBAM_WorldData::handleModificationSubclass (int code){
	//LOG_INDENT;
	//LOG("SIM_PhysBAM_WorldData::handleModificationSubclass() called.");
	
	//LOG("SIM_PhysBAM_WorldData::handleModificationSubclass(" << code << ")");
	BaseClass::handleModificationSubclass(code);
	
	//LOG("Done");
	//LOG_UNDENT;		
}

physbam_force*
SIM_PhysBAM_WorldData::addNewForce(const SIM_Data *force){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::addNewForce(SIM_Data *force) called.");
	
	physbam_force *pb_force = NULL;
	const UT_String force_type = force->getDataType();
	
	if(force_type == "SIM_ForceGravity"){
		LOG("Adding force: " << force_type);
		const SIM_ForceGravity	*gravity = SIM_DATA_CASTCONST(force, SIM_ForceGravity);
		
		data_exchange::gravity_force gf;
		pb_force = ir.add_force(getSimulation(), &gf);
		
		UT_Vector3 gravity_direction = gravity->getGravity();
		data_exchange::vf3 dir(gravity_direction[0], gravity_direction[1], gravity_direction[2]); 
		
		ir.set_float(pb_force, ir.get_id(pb_force, "magnitude"), 1);
		ir.set_vf3(pb_force, ir.get_id(pb_force, "direction"), dir);
	}else{
		LOG("Skipping unsupported force: " << force_type);
	}
	
	LOG("Done");
	LOG_UNDENT;
	return pb_force;		
}

physbam_object*
SIM_PhysBAM_WorldData::addNewObject(SIM_Object *object, SIM_Time time){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::addNewObject(const SIM_Object *object) called.");
	
	physbam_object *pb_object = NULL;
	SIM_SopGeometry* geometry;
	
	geometry = SIM_DATA_CAST(object->getNamedSubData("Geometry"), SIM_SopGeometry);
	
    if (!geometry)
    {
		LOG("This object has no geometry. Skipping !!!");
		LOG_UNDENT;
    }else{
		
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
		HPI_TriMesh	*trimesh = new HPI_TriMesh();
		
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
		
		pb_object = ir.add_object(getSimulation(), &db);
		
		if(pb_object){
			//~ {
				//~ int xid = ir.get_id(pb_object, "position");
				//~ int len = ir.get_vf3_array_length(pb_object, xid);
				//~ std::vector<data_exchange::vf3> x_array(len);
				//~ ir.get_vf3_array(pb_object, xid, &x_array[0], len, 0);
				//~ for(size_t i=0; i<x_array.size(); i++)
				//~ printf("(%g %g %g)\n", x_array[i].data[0], x_array[i].data[1], x_array[i].data[2]);
			//~ }
			LOG("PhysBAM object " << object->getObjectId() << " added to worlddata with pointer: " << pb_object ); 		
			trimeshes->insert( std::pair<int, HPI_TriMesh*>(object->getObjectId(), trimesh));
			objects->insert( std::pair<int, physbam_object*>(object->getObjectId(), pb_object));
			
			/// Now create constraints
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

			/*
			SIM_DataFilterByType 	anchorFilter("SIM_Constraint");
			SIM_ConstraintIterator 	conit(*object, 0, &anchorFilter, &anchorFilter, time);
            const SIM_Constraint	*constraint = 0;
            
            for (conit.rewind(); !conit.atEnd(); conit.advance())
			{
				constraint = conit.getConstraint();
				LOG("Constraint: " << constraint->getDataType() << std::endl);
			}*/                
                            
			/// Create volume force for deformable body
			LOG("Creating PhysBAM volumetric force for object: " << object->getObjectId());
			data_exchange::volumetric_force vf;
			vf.stiffness = stiffness;
			vf.damping = damping;
			physbam_force* f1 = ir.add_force(getSimulation(), &vf);
			ir.apply_force_to_object(pb_object, f1);
		}
	}
	
	LOG("Done");
	LOG_UNDENT;
	return pb_object;	
}

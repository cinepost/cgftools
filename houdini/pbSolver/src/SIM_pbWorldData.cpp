#include "SIM_pbWorldData.h"

extern interface_routines ir;

#include <SIM/SIM_Data.h>
#include <SIM/SIM_DataFactory.h>
#include "logtools.h"

SIM_PhysBAM_WorldData::SIM_PhysBAM_WorldData(const SIM_DataFactory *factory) : SIM_Data(factory),
	simulation(0),
	objects(0),
	m_shareCount(0){

};

SIM_PhysBAM_WorldData::~SIM_PhysBAM_WorldData(){ 
	clear();
};

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
		LOG("SIM_PhysBAM_WorldData::getSimulation() using existing simulation instace: " << simulation);
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
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::initializeSubclass() called.");
	clear();
	simulation		= NULL;
	objects			= new std::map<int, physbam_object*>();
	forces			= new std::map<int, physbam_force*>();
	m_shareCount	= new int(1);
	LOG("Done");
	LOG_UNDENT;
};

void SIM_PhysBAM_WorldData::makeEqualSubclass(const SIM_Data *src){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::makeEqualSubclass() called.");
	const SIM_PhysBAM_WorldData *world;
	world = SIM_DATA_CASTCONST(src, SIM_PhysBAM_WorldData);
	if( world ){
		clear();
		simulation		= world->simulation;	
		objects 		= world->objects;	
		forces 			= world->forces;					
		m_shareCount 	= world->m_shareCount;
		(*m_shareCount)++;
	}
	LOG("Done");
	LOG_UNDENT;	
}

void SIM_PhysBAM_WorldData::clear(){
	LOG_INDENT;
	if( m_shareCount ){
		(*m_shareCount)--;
		if(*m_shareCount == 0){
			
			if(simulation){
				LOG("SIM_PhysBAM_WorldData::clear() destroying sim: " << simulation);
				ir.destroy_simulation(simulation);
			}
			delete	objects;				
			delete 	m_shareCount;
		}
	}
	simulation		= 0;
	objects			= 0;		
	m_shareCount 	= 0;
	LOG_UNDENT;	
}

void 	
SIM_PhysBAM_WorldData::handleModificationSubclass (int code){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::handleModificationSubclass() called.");
	BaseClass::handleModificationSubclass(code);
	LOG("Done");
	LOG_UNDENT;		
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
SIM_PhysBAM_WorldData::addNewObject(SIM_Object *object){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::addNewObject(const SIM_Object *object) called.");
	
	physbam_object *pb_object = NULL;
	SIM_pbGeometry* pb_geometry;
	
	pb_geometry = SIM_DATA_CAST(object->getNamedSubData("PhysBAM_Geometry"), SIM_pbGeometry);
	
    if (!pb_geometry)
    {
		LOG("This object has no geometry. Skipping !!!");
		LOG_UNDENT;
    }else{

		SIM_Data				*body_data = object->getNamedSubData("PhysBAM_Body");
		
		float 	stiffness = 1e3;
		float 	damping = .01;
		int 	approx_number_cells = 100;		
		
		/*
		if(body_data){
			const SIM_PhysBAM_Body 	*physbam_body = SIM_DATA_CASTCONST(body_data, SIM_PhysBAM_Body);
			LOG("Fetching options from PhysBAM_Body object data.");
			
			LOG("DEFORMABLE:" << physbam_body->getDeformable());
			LOG("STIFFNESS: " << physbam_body->getStiffness());
			LOG("DAMPING: " << physbam_body->getDamping());
			LOG("APPROX CELLS NUMBER: " << physbam_body->getCellsApprox());	
		}*/
		
		const GU_Detail*				const gdp(pb_geometry->getGeometry().readLock());
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
	
		db.approx_number_cells = approx_number_cells;
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
			pb_geometry->setMesh(trimesh);		
			objects->insert( std::pair<int, physbam_object*>(object->getObjectId(), pb_object));
			
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

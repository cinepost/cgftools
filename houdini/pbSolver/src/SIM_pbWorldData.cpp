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

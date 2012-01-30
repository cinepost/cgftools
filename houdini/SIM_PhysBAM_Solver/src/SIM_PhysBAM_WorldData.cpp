#include "SIM_PhysBAM_WorldData.h"

extern interface_routines ir;

#include <SIM/SIM_Data.h>
#include <SIM/SIM_DataFactory.h>
#include "logtools.h"

SIM_PhysBAM_WorldData::SIM_PhysBAM_WorldData(const SIM_DataFactory *factory) : SIM_Data(factory),
	simulation(NULL), objects(0), forces(0), m_shareCount(0){

};

SIM_PhysBAM_WorldData::~SIM_PhysBAM_WorldData(){ 
	clear();
};

physbam_force*
SIM_PhysBAM_WorldData::getForce(int id){
	return forces->find(id)->second;
}

HPI_Object*
SIM_PhysBAM_WorldData::getSolidObject(int id){
	return objects->find(id)->second;
}

std::map<int, physbam_force*>*
SIM_PhysBAM_WorldData::getForces(void){
	return forces;
}

std::map<int, HPI_Object*>*
SIM_PhysBAM_WorldData::getObjects(void){
	return objects;
}

bool
SIM_PhysBAM_WorldData::forceExists(int id){
	return forces->find( id ) != forces->end();
}

bool
SIM_PhysBAM_WorldData::objectExists(int id){
	return objects->find( id ) != objects->end();
}

physbam_simulation* 
SIM_PhysBAM_WorldData::getSimulation(void){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::getSimulation() called.");
	if(!simulation){
		LOG("SIM_PhysBAM_WorldData::getSimulation() creating new simulation instace.");
		simulation	= ir.call<physbam_simulation *>("create_simulation", "solids");
		LOG("SIM_PhysBAM_WorldData::getSimulation() created instance: "  << simulation);
		ir.call<void>("set_output_directory", simulation, "/opt/houdini/output/");
		ir.call<void>("set_data_directory", simulation, "/opt/PhysBAM-2011/Public_Data");
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
	forces			= new std::map<int, physbam_force*>();
	objects			= new std::map<int, HPI_Object*>();
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
				ir.call<void>("destroy_simulation", simulation);
			}
			for ( std::map<int, HPI_Object*>::iterator it = objects->begin(); it != objects->end(); ++it ){delete it->second;};objects->clear();
			for ( std::map<int, physbam_force*>::iterator it = forces->begin(); it != forces->end(); ++it ){delete it->second;};forces->clear();

			delete 	m_shareCount;
		}
	}
	simulation		= NULL;
	objects			= NULL;
	forces			= NULL;
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
				
		UT_Vector3 gravity_direction = gravity->getGravity();
		vf3 dir(gravity_direction[0], gravity_direction[1], gravity_direction[2]); 
		
		pb_force = ir.call<physbam_force*>("add_gravity", getSimulation(), dir);
		
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
	LOG("SIM_PhysBAM_WorldData::addNewObject(SIM_Object *object, SIM_Time time) called.");	
	SIM_SopGeometry* 	geometry = SIM_DATA_CAST(object->getNamedSubData("Geometry"), SIM_SopGeometry);
    if(geometry){
		SIM_SDF	*volume = NULL;
		volume = SIM_DATA_CAST(geometry->getNamedSubData("Volume"), SIM_SDF);
		if(volume){
			LOG("Adding volumetric object...");
			if( volume->getMode() == 4){
				LOG("Done");
				LOG_UNDENT;
				return addNewGroundObject(object);	
			}else if( volume->getMode() == 0){
				LOG("Done");
				LOG_UNDENT;
				return addNewBakedObject(object);					
			}
		}
		LOG("Done");
		LOG_UNDENT;
		return addNewSolidObject(object);
	}
	LOG("Unable to add object:" << object->getObjectId());
	LOG_UNDENT;
	return 0;
}

physbam_object*
SIM_PhysBAM_WorldData::addNewGroundObject(SIM_Object *object){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::addNewGroundObject(SIM_Object *object) setting up simple ground:");
	HPI_Ground_Object	*obj = new HPI_Ground_Object();
	if( obj->setFromObject(object, getSimulation())){
		/// Add id-object pair 
		objects->insert( std::pair<int, HPI_Object*>(object->getObjectId(), obj));
		
		LOG("Done");
		LOG_UNDENT;
		return obj->getPhysbamObject();
	}
	
	LOG("Done");
	LOG_UNDENT;
	return 0;
}

physbam_object*
SIM_PhysBAM_WorldData::addNewSolidObject(SIM_Object *object){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::addNewSolidObject(SIM_Object *object) setting up solid object:");	
	HPI_Solid_Object	*obj = new HPI_Solid_Object();
	if( obj->setFromObject(object, getSimulation())){

		/// Add id-object pair 
		objects->insert( std::pair<int, HPI_Object*>(object->getObjectId(), obj));

		/// Add affector objects
		SIM_ObjectArray		sim_objects, filtered;
		object->getAffectors(sim_objects, "SIM_RelationshipCollide");
		sim_objects.filter(SIM_DataFilterByType("SIM_Object"), filtered);	/// we want to process only certain types of objects here
		
		if(filtered.entries() > 0){
			LOG("Setting up " << filtered.entries() << " affectors...");
			
			SIM_Object *sim_obj;
			for(int i = 0; i < filtered.entries(); i++){
				
				sim_obj = (SIM_Object*)filtered(i);
				if(object->getObjectId() != sim_obj->getObjectId()){
					if(!objectExists(sim_obj->getObjectId())){
						
						LOG("Setup affector: " << sim_obj->getName());
						addNewObject(sim_obj, 0);
					}
				}
			}
		}
		
		
		/// Add object related forces
		const SIM_Data	*force;
		SIM_ConstDataArray forces;
	
		object->filterConstSubData( forces, 0, SIM_DataFilterByType("SIM_Force"), SIM_FORCES_DATANAME, SIM_DataFilterNone() );
		if(!forces.isEmpty()){
			LOG("Creating PhysBAM forces ...");
			physbam_force	*pb_force;
			int force_id	= 0;
			
			for(int i=0; i < forces.entries(); i++){
				pb_force = NULL;
				force = forces[i];
				force_id = force->getCreatorId();
				if(!forceExists(force_id)){
					/// Add this force to world data
					pb_force = addNewForce(force);
				}else{
					/// Use existing force
					LOG("Bind existing force " << force->getDataType());	
					pb_force = getForce(force_id);
				}
				
				if(pb_force)
					ir.call<void>("apply_force_to_object", obj->getPhysbamObject(), pb_force);	
			}
		}

		LOG("Done");
		LOG_UNDENT;
		return obj->getPhysbamObject();
	}
	LOG("Done");
	LOG_UNDENT;
	return 0;	
}

physbam_object*
SIM_PhysBAM_WorldData::addNewBakedObject(SIM_Object *object){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::addNewBakedObject(SIM_Object *object) setting up scripted object:");	
	HPI_Baked_Object	*obj = new HPI_Baked_Object();
	if( obj->setFromObject(object, getSimulation())){	
		/// Add id-object pair 
		objects->insert( std::pair<int, HPI_Object*>(object->getObjectId(), obj));
		LOG("Done");
		LOG_UNDENT;
		return obj->getPhysbamObject();
	}
	LOG("Done");
	LOG_UNDENT;
	return 0;	
}

#include "SIM_PhysBAM_WorldData.h"

extern interface_routines ir;

#include <SIM/SIM_Data.h>
#include <SIM/SIM_DataFactory.h>
#include "logtools.h"

SIM_PhysBAM_WorldData::SIM_PhysBAM_WorldData(const SIM_DataFactory *factory) : SIM_Data(factory),
	simulation(NULL), simulations(0),
	objects(0), fluid_objects(0), solid_objects(0),
	forces(0),
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

HPI_Fluid_Object*
SIM_PhysBAM_WorldData::getFluidObject(int id){
	return fluid_objects->find(id)->second;
}

HPI_Solid_Object*
SIM_PhysBAM_WorldData::getSolidObject(int id){
	return solid_objects->find(id)->second;
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
SIM_PhysBAM_WorldData::fluidObjectExists(int id){
	return fluid_objects->find( id ) != fluid_objects->end();
}

bool
SIM_PhysBAM_WorldData::solidObjectExists(int id){
	return solid_objects->find( id ) != solid_objects->end();
}

bool
SIM_PhysBAM_WorldData::simulationExists(unsigned char type){
	return simulations->find( type ) != simulations->end();
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

physbam_simulation* 
SIM_PhysBAM_WorldData::getSimulation(int uid, unsigned char type){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::getSimulation(unsigned char type) called.");
	physbam_simulation* sim = NULL;
	
	if(simulationExists(uid)){
		sim = simulations->find(uid)->second;
		LOG("SIM_PhysBAM_WorldData::getSimulation(unsigned char type) using existing simulation instance: " << sim);
	}else{
		LOG("SIM_PhysBAM_WorldData::getSimulation() creating new simulation instace.");

		switch(type)
		{
		case SOLID_TYPE:
			sim	= ir.create_simulation("solids");
			ir.set_string(sim, ir.get_id(sim, "output_directory"), "solid-output");		
			break;
		case SMOKE_TYPE:
			sim = ir.create_simulation("smoke");
			ir.set_string(sim, ir.get_id(sim, "output_directory"), "smoke-output");
			ir.set_int(sim, ir.get_id(sim, "use_mgpcg"), 1);		
			break;
		case WATER_TYPE:
			sim = ir.create_simulation("water");
			ir.set_string(sim, ir.get_id(sim, "output_directory"), "water-output");
			ir.set_int(sim, ir.get_id(sim, "use_mgpcg"), 1);		
			break;
		default:
			break;
		}	
			
		if(sim != NULL){
			LOG("SIM_PhysBAM_WorldData::getSimulation(unsigned char type) created instance: "  << sim);
			ir.set_string(sim, ir.get_id(sim, "data_directory"), "/opt/PhysBAM-2011/Public_Data");
			simulations->insert( std::pair<int, physbam_simulation*>(uid, sim));	
		}else{
			LOG("SIM_PhysBAM_WorldData::getSimulation(unsigned char type) ERROR CREATING SIMULATION INSTANCE!!!");
		}	
	}
	
	LOG("Done");
	LOG_UNDENT; 	
	return sim;		
}

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
	simulations		= new std::map<int, physbam_simulation*>();
	objects			= new std::map<int, physbam_object*>();
	forces			= new std::map<int, physbam_force*>();
	fluid_objects	= new std::map<int, HPI_Fluid_Object*>();
	solid_objects	= new std::map<int, HPI_Solid_Object*>();
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
		simulations		= world->simulations;	
		objects 		= world->objects;
		fluid_objects 	= world->fluid_objects;	
		solid_objects 	= world->solid_objects;
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
				ir.destroy_simulation(simulation);
			}
			for ( typename std::map<int, physbam_simulation*>::iterator it = simulations->begin(); it != simulations->end(); ++it ){delete it->second; };simulations->clear();
			for ( typename std::map<int, physbam_object*>::iterator it = objects->begin(); it != objects->end(); ++it ){delete it->second; };objects->clear();
			for ( typename std::map<int, HPI_Fluid_Object*>::iterator it = fluid_objects->begin(); it != fluid_objects->end(); ++it ){delete it->second;};fluid_objects->clear();
			for ( typename std::map<int, HPI_Solid_Object*>::iterator it = solid_objects->begin(); it != solid_objects->end(); ++it ){delete it->second;};solid_objects->clear();
			for ( typename std::map<int, physbam_force*>::iterator it = forces->begin(); it != forces->end(); ++it ){delete it->second;};forces->clear();

			delete 	m_shareCount;
		}
	}
	simulation		= NULL;
	simulations		= NULL;
	objects			= NULL;
	forces			= NULL;
	fluid_objects	= NULL;
	solid_objects 	= NULL;
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
	const SIM_Position *sim_pos = object->getPosition();
	UT_Vector3 		t, n;
	UT_DMatrix4		xform;
	UT_Matrix3		rot;
	
	//sim_pos->getPosition(t);
	//sim_pos->getInverseTransform(xform);
	sim_pos->getTransform(xform);
	
	xform.extractRotate(rot);	
	xform.getTranslates(t);
	
	n = UT_Vector3 (0.0f ,1.0f ,0.0f);
	
	LOG("N: " << n.x() << " " << n.y() << " " << n.z());
	n *= rot;
	LOG("N: " << n.x() << " " << n.y() << " " << n.z());
	LOG("T: " << t.x() << " " << t.y() << " " << t.z());
	
	data_exchange::ground_plane gp;
	gp.position = data_exchange::vf3(t.x(), t.y(), t.z());
	gp.normal 	= data_exchange::vf3(n.x(), n.y(), n.z());
	LOG("Done");
	LOG_UNDENT;
	return ir.add_object(getSimulation(0, SOLID_TYPE), &gp);
}

physbam_object*
SIM_PhysBAM_WorldData::addNewSolidObject(SIM_Object *object){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::addNewSolidObject(SIM_Object *object) setting up solid object:");	
	HPI_Solid_Object	*obj = new HPI_Solid_Object();
	if( obj->setFromObject(object, getSimulation(0, SOLID_TYPE))){
		/// Add affector objects
		SIM_ObjectArray		objects;		
		object->getAffectors(objects, "SIM_RelationshipCollide");
		
		if(objects.entries() > 0){
			LOG("Setting up " << objects.entries() << " affectors...");
			SIM_Object *obj;
			for(int i = 0; i < objects.entries(); i++){
				obj = (SIM_Object*)objects(i);
				if(object->getObjectId() != obj->getObjectId()){
					LOG("Setup affector: " << obj->getName());
					addNewObject(obj, 0);
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
					ir.apply_force_to_object(obj->getPhysbamObject(), pb_force);		
			}
		}
		/// Add id-object pair 
		solid_objects->insert( std::pair<int, HPI_Solid_Object*>(obj->getUid(), obj));
		LOG("Done");
		LOG_UNDENT;
		return obj->getPhysbamObject();
	}
	LOG("Done");
	LOG_UNDENT;
	return 0;	
}

bool
SIM_PhysBAM_WorldData::addNewFluidObject(SIM_Object *object, SIM_Time time){
	LOG_INDENT;
	LOG("SIM_PhysBAM_WorldData::addNewFluidObject(const SIM_Object *object) called.");
	
	UT_Vector3			size;
	UT_Vector3			divisions;
	SIM_SopScalarField* primary = NULL;
	SIM_SopScalarField* density = NULL;
	SIM_SopScalarField* surface = NULL;
	SIM_SopVectorField* velocity = NULL;
	physbam_simulation* sim = NULL;
	HPI_Fluid_Object	*fluid_object = new HPI_Fluid_Object();
	density  = SIM_DATA_CAST(object->getNamedSubData("density"), SIM_SopScalarField);
	surface  = SIM_DATA_CAST(object->getNamedSubData("surface"), SIM_SopScalarField);
	velocity = SIM_DATA_CAST(object->getNamedSubData("vel"), SIM_SopVectorField);
	
	if(density){
		fluid_object->setFluidType(SMOKE_TYPE);
		sim = getSimulation(object->getObjectId(), SMOKE_TYPE);
		primary = density;
		LOG("Adding smoke type object...");
	}else if(surface){
		fluid_object->setFluidType(WATER_TYPE);
		sim = getSimulation(object->getObjectId(), WATER_TYPE);
		primary = surface;
		LOG("Adding water type object...");
	}else{
		LOG("Unknown fluid object type !!! Skipping ...");
		LOG_UNDENT;	
		return false;
	}
	
	divisions = primary->getDivisions();
	size = primary->getSize();
	
	fluid_object->setSize(size.x(), size.y(), size.z());
	fluid_object->setDivisions(divisions.x(), divisions.y(), divisions.z());

	fluid_objects->insert( std::pair<int, HPI_Fluid_Object*>(object->getObjectId(), fluid_object));
	
	data_exchange::fluid_domain fd;
    fd.position.push_back(data_exchange::vf3(0,0,0));
    fd.position.push_back(data_exchange::vf3(0,0,2));
    fd.position.push_back(data_exchange::vf3(0,2,0));
    fd.position.push_back(data_exchange::vf3(0,2,2));
    fd.position.push_back(data_exchange::vf3(2,0,0));
    fd.position.push_back(data_exchange::vf3(2,0,2));
    fd.position.push_back(data_exchange::vf3(2,2,0));
    fd.position.push_back(data_exchange::vf3(2,2,2));
    
    fd.mesh.insert_polygon(data_exchange::vi4(7,6,2,3));
    fd.mesh.insert_polygon(data_exchange::vi4(2,6,4,0));
    fd.mesh.insert_polygon(data_exchange::vi4(1,0,4,5));
    fd.mesh.insert_polygon(data_exchange::vi4(0,1,3,2));
    fd.mesh.insert_polygon(data_exchange::vi4(3,1,5,7));
    fd.mesh.insert_polygon(data_exchange::vi4(4,6,7,5));

    fd.is_wall.push_back(0);
    fd.is_wall.push_back(1);
    fd.is_wall.push_back(1);
    fd.is_wall.push_back(1);
    fd.is_wall.push_back(1);
    fd.is_wall.push_back(1);

    fd.resolution=64;
    ir.add_object(sim, &fd);
    
    data_exchange::fluid_grid_data grid_data;
    ir.get_fluid_grid_data(sim, grid_data);    
    
    printf("grid_data %g %g %g   %g   %i %i %i\n",
        grid_data.corner_location.data[0], grid_data.corner_location.data[1], grid_data.corner_location.data[2],
        grid_data.dx, grid_data.dimensions.data[0], grid_data.dimensions.data[1], grid_data.dimensions.data[2]);    
    
	data_exchange::vf3 box_a(.9,-.2,.9), box_b(1.1,.2,1.1);
	data_exchange::vi3 index_a, index_b;
	for(int i=0;i<3;i++)
	{
		index_a.data[i] = std::max(0, (int) ceil((box_a.data[i] - grid_data.corner_location.data[i])/grid_data.dx));
		index_b.data[i] = std::min(grid_data.dimensions.data[i]-1, (int) floor((box_b.data[i] - grid_data.corner_location.data[i])/grid_data.dx));
	}
    
    std::vector<data_exchange::vi3> source_cells;
    std::vector<data_exchange::vf3> source_velocities;
    std::vector<float> source_densities;


	
	SIM_RawField *field = primary->getField();
	//SIM_RawField *vel_x = velocity->getField(0);
	//SIM_RawField *vel_y = velocity->getField(1);
	//SIM_RawField *vel_z = velocity->getField(2);
	
	
	for(int i=0;i<divisions.x();i++){
		for(int j=0;j<divisions.y();j++){
			for(int k=0;k<divisions.z();k++){
				source_cells.push_back(data_exchange::vi3(i,j,k));
				//source_velocities.push_back(data_exchange::vf3(vel_x->getCellValue(i,j,k),vel_y->getCellValue(i,j,k),vel_z->getCellValue(i,j,k)));
				source_velocities.push_back(data_exchange::vf3(0,.5,0));
				source_densities.push_back(field->getCellValue(i,j,k));
			}
		} 
	}
	
	
	//printf("index data %i %i %i   %i %i %i\n",
    //    index_a.data[0], index_a.data[1], index_a.data[2],
    //    index_b.data[0], index_b.data[1], index_b.data[2]); 
		
	//for(int i=index_a.data[0];i<=index_b.data[0];i++)
	//	for(int j=index_a.data[1];j<=index_b.data[1];j++)
	//		for(int k=index_a.data[2];k<=index_b.data[2];k++)
	//		{
	//			source_cells.push_back(data_exchange::vi3(i,j,k));
	//			source_velocities.push_back(data_exchange::vf3(0,.5,0));
	//			source_densities.push_back(10);
	//		}
		
    ir.set_vi3_array(sim, ir.get_id(sim, "source_cells"), &source_cells[0], source_cells.size(), 0);
    ir.set_vf3_array(sim, ir.get_id(sim, "source_velocities"), &source_velocities[0], source_velocities.size(), 0);
    ir.set_float_array(sim, ir.get_id(sim, "source_densities"), &source_densities[0], source_densities.size(), 0);   
    
	data_exchange::scripted_geometry sg;
	sg.position.push_back(data_exchange::vf3(1.5,1,1));
	sg.position.push_back(data_exchange::vf3(.5,1,1));
	sg.position.push_back(data_exchange::vf3(1,1.5,1));
	sg.position.push_back(data_exchange::vf3(1,.5,1));
	sg.position.push_back(data_exchange::vf3(1,1,1.5));
	sg.position.push_back(data_exchange::vf3(1,1,.5));
	sg.mesh.insert_polygon(data_exchange::vi3(0,2,4));
	sg.mesh.insert_polygon(data_exchange::vi3(4,2,1));
	sg.mesh.insert_polygon(data_exchange::vi3(0,5,2));
	sg.mesh.insert_polygon(data_exchange::vi3(4,3,0));
	sg.mesh.insert_polygon(data_exchange::vi3(1,3,4));
	sg.mesh.insert_polygon(data_exchange::vi3(5,0,3));
	sg.mesh.insert_polygon(data_exchange::vi3(2,5,1));
	sg.mesh.insert_polygon(data_exchange::vi3(1,5,3));
	physbam_object* d2 = ir.add_object(sim, &sg);
	(void)d2; 
	
	LOG("Done");
	LOG_UNDENT;	
	return true;	
}

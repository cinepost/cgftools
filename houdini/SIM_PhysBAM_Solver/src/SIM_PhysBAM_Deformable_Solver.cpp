#include "SIM_PhysBAM_Deformable_Solver.h"

extern interface_routines ir;

struct physbam_simulation;
struct physbam_object;
struct physbam_force;

extern	bool	PB_LIB_READY;	
	
SIM_PhysBAM_Deformable_Solver::SIM_PhysBAM_Deformable_Solver(const SIM_DataFactory *factory) : BaseClass(factory), SIM_OptionsUser(this){
	
};

SIM_PhysBAM_Deformable_Solver::~SIM_PhysBAM_Deformable_Solver(){
	
};

const SIM_DopDescription* SIM_PhysBAM_Deformable_Solver::getSolverDopDescription(){
    static PRM_Template  theTemplates[] = {
        PRM_Template()
    };

    static SIM_DopDescription    theDopDescription(true, "pbm_deformable_solver", "PhysBAM Deformable Solver", SIM_SOLVER_DATANAME, classname(), theTemplates);
    return &theDopDescription;
};	

bool SIM_PhysBAM_Deformable_Solver::setupNewSimObject(physbam_simulation* sim, SIM_Object* object, SIM_Time time){
	LOG_INDENT;
	LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() called.");
	LOG(object->getName() << " id: " << object->getObjectId());
	
	SIM_Data*	physbam_body = NULL;
	physbam_body = object->getNamedSubData("PhysBAM_Body");
    
    if(physbam_body){
		LOG("Creating PhysBAM object...");
		physbam_object* pb_object = worlddata->addNewObject(object, time);
		
		if(pb_object){
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
					if(!worlddata->forceExists(force_id)){
						/// Add this force to world data
						pb_force = worlddata->addNewForce(force);
					}else{
						/// Use existing force
						LOG("Bind existing force " << force->getDataType());	
						pb_force = worlddata->getForce(force_id);
					}
					
					if(pb_force)
						ir.apply_force_to_object(pb_object, pb_force);		
				}
			}
			return true;
		}else{
			printf("Unable to add new object %d into simulation !!!\n", object->getObjectId());
			return false;
		}
	}
	return false;
}
bool SIM_PhysBAM_Deformable_Solver::updateSimObject(physbam_simulation* sim, SIM_Object* object){
	std::cout << "\t" << object->getName() << " id: " << object->getObjectId() << std::endl;
	SIM_GeometryCopy	*geometry_copy = 0;
	SIM_Geometry		*geo = 0;
	
	geo = SIM_DATA_CAST(object->getNamedSubData("Geometry"), SIM_Geometry);
	
	if(geo!=0){
		physbam_object* pb_object = worlddata->getSolidObject(object->getObjectId())->getPhysbamObject();
		if(pb_object){
			int xid = ir.get_id(pb_object, "position");
			int len = ir.get_vf3_array_length(pb_object, xid);
			std::vector<data_exchange::vf3> x_array(len);
			ir.get_vf3_array(pb_object, xid, &x_array[0], len, 0);
			
			HPI_TriMesh		*trimesh = worlddata->getSolidObject(object->getObjectId())->getTrimesh();
			trimesh->setToObject(object);
		}
	}else{
		std::cout << "Unable to get Geometry for object: " << object->getObjectId() << " to update!" <<std::endl;
	}
	return true;
}
	
SIM_Solver::SIM_Result
SIM_PhysBAM_Deformable_Solver::solveObjectsSubclass ( SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep){
	LOG_INDENT;
	LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() called.");		
	bool new_world = false;
	worlddata = SIM_DATA_GET(*this, "PhysBAM_World", SIM_PhysBAM_WorldData);
	if(!worlddata){
		LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() creating new PhysBAM_World data.");
		new_world = true;		
		worlddata = SIM_DATA_CREATE(*this, "PhysBAM_World", SIM_PhysBAM_WorldData, SIM_DATA_RETURN_EXISTING);
		if(!worlddata){
			LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() unable to get PhysBAM_World data.");
			LOG_UNDENT;
			return SIM_Solver::SIM_SOLVER_FAIL;
		}
	}
		
	OP_Node*				obj_parent;
	const SIM_Object*		obj;
	const SIM_Geometry*		sourcegeo;
	SIM_ColliderInfoArray	colliderinfo;  
	SIM_ObjectArray			filtered;
	SIM_Time				sim_time;
	UT_String				group, simfilename;
	physbam_simulation* 	sim;
	int						i;
	const SIM_Time 			curr_time = engine.getSimulationTime();
	
	sim = worlddata->getSimulation(0, SOLID_TYPE);
	
	if (new_world) {
		// setup basic forces and ground
		LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() setting up simple ground:");
		data_exchange::ground_plane gp;
		gp.position = data_exchange::vf3(0,-10,0);
		gp.normal = data_exchange::vf3(0,1,0);
		ir.add_object(sim, &gp);
	}
	
	/// Loop through new objects and add them into sim.
	if (newobjects.entries() > 0) {
		LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() setting up new objects in sim:");	
		for( i = 0; i < newobjects.entries(); i++ ){	
	    	if(!setupNewSimObject(sim, newobjects(i), curr_time)){
				LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() unable set up new object:" << newobjects(i)->getName());
				LOG_UNDENT;
				return SIM_Solver::SIM_SOLVER_FAIL;
			}
		}
		/// exit with success. we don't want to run actual simulation here
		return SIM_Solver::SIM_SOLVER_SUCCESS;	
	}
	
	/// Run the simulation for the given time_step
	LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() running ir.simulate_frame with sim:" << sim << " and timestep: " << timestep);	
	ir.simulate_frame(sim, timestep);
	LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() simulated.");	
	
	/// Update all the objects
	if (objects.entries() > 0){
		LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() updating objects in sim:");
		for( i = 0; i < objects.entries(); i++ ){ 
	    	if(!updateSimObject(sim, objects(i))){
				LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() unable update object:" << objects(i)->getName());
				LOG_UNDENT;
				return SIM_Solver::SIM_SOLVER_FAIL;
			}  
		}	
	}
	LOG_UNDENT;	
	LOG("Done");		
	return SIM_Solver::SIM_SOLVER_SUCCESS;
}	

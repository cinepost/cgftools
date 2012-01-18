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
	LOG("SIM_PhysBAM_Deformable_Solver:setupNewSimObject(physbam_simulation* sim, SIM_Object* object, SIM_Time time) called.");
	
	LOG(object->getName() << " id: " << object->getObjectId());
	
	physbam_object* pb_object = worlddata->addNewObject(object, time);
	if(pb_object){
		LOG("Done.");
		LOG_UNDENT;
		return true;		
	}
	LOG("Done.");
	LOG_UNDENT;
	return false;	
}

bool SIM_PhysBAM_Deformable_Solver::updateSimObject(physbam_simulation* sim, SIM_Object* object, const SIM_Time &timestep){
	bool result = worlddata->getSolidObject(object->getObjectId())->updateSimulatedObject(object, sim, timestep);
	return result;
}
	
SIM_Solver::SIM_Result
SIM_PhysBAM_Deformable_Solver::solveObjectsSubclass ( SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep){
	LOG_INDENT;
	LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() called.");		
	UT_Interrupt	*boss;
	boss = UTgetInterrupt();
	if (boss->opStart("Running PhysBAM solver ..."))
	{
		bool new_world = false;
		worlddata = SIM_DATA_GET(*this, "PhysBAM_World", SIM_PhysBAM_WorldData);
		if(!worlddata){
			LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() creating new PhysBAM_World data.");
			new_world = true;		
			worlddata = SIM_DATA_CREATE(*this, "PhysBAM_World", SIM_PhysBAM_WorldData, SIM_DATA_RETURN_EXISTING);
			if(!worlddata){
				LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() unable to get PhysBAM_World data.");
				LOG_UNDENT;
				boss->opEnd();
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
		
		sim = worlddata->getSimulation();
		/// Loop through new objects and add them into sim.
		if (newobjects.entries() > 0) {	
			for( i = 0; i < newobjects.entries(); i++ ){
				if(!worlddata->objectExists(newobjects(i)->getObjectId())){
					LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() setting up new object in sim: " << newobjects(i)->getName());	
					if(!setupNewSimObject(sim, newobjects(i), curr_time)){
						LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() unable set up new object:" << newobjects(i)->getName());
						LOG_UNDENT;
						boss->opEnd();
						return SIM_Solver::SIM_SOLVER_FAIL;
					}
				}
			}
			/// exit with success. we don't want to run actual simulation here
			boss->opEnd();
			return SIM_Solver::SIM_SOLVER_SUCCESS;	
		}
		
		/// Run the simulation for the given time_step
		LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() running ir.simulate_frame with sim:" << sim << " and timestep: " << timestep);	
		ir.call<void>("simulate_frame", sim, (float)timestep);
		LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() simulated.");	
		
		/// Update all the objects
		if (objects.entries() > 0){
			LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() updating objects in sim: " << sim);
			for( i = 0; i < objects.entries(); i++ ){ 
				LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() updating object \"" << objects(i)->getName() << "\"");
		    	if(!updateSimObject(sim, objects(i), timestep)){
					LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() unable update object \"" << objects(i)->getName() << "\"");
					LOG_UNDENT;
					boss->opEnd();
					return SIM_Solver::SIM_SOLVER_FAIL;
				}else{
					LOG("SIM_PhysBAM_Deformable_Solver solveObjectsSubclass() object \"" << objects(i)->getName() << "\" updated successfully !!!");
				}  
			}	
		}
		LOG_UNDENT;	
		LOG("Done");
		boss->opEnd();		
		return SIM_Solver::SIM_SOLVER_SUCCESS;
	}
}	

#include "SIM_PhysBAM_Fluid_Solver.h"

extern interface_routines ir;

struct physbam_simulation;
struct physbam_object;
struct physbam_force;

extern	bool	PB_LIB_READY;	
	
SIM_PhysBAM_Fluid_Solver::SIM_PhysBAM_Fluid_Solver(const SIM_DataFactory *factory) : BaseClass(factory), SIM_OptionsUser(this){
	
};

SIM_PhysBAM_Fluid_Solver::~SIM_PhysBAM_Fluid_Solver(){
	
};

const SIM_DopDescription* SIM_PhysBAM_Fluid_Solver::getSolverDopDescription(){
    static PRM_Template  theTemplates[] = {
        PRM_Template()
    };

    static SIM_DopDescription    theDopDescription(true, "pbm_fluid_solver", "PhysBAM Fluid Solver", SIM_SOLVER_DATANAME, classname(), theTemplates);
    return &theDopDescription;
};	

bool SIM_PhysBAM_Fluid_Solver::setupNewSimObject(physbam_simulation* sim, SIM_Object* object, SIM_Time time){
	LOG_INDENT;
	LOG("SIM_PhysBAM_Fluid_Solver solveObjectsSubclass() called.");
	LOG(object->getName() << " id: " << object->getObjectId());

	worlddata->addNewFluidObject(object, time);

	LOG("Done.");
	LOG_UNDENT;
	return true;
}
bool SIM_PhysBAM_Fluid_Solver::updateSimObject(physbam_simulation* sim, SIM_Object* object){
	LOG_INDENT;
	LOG("SIM_PhysBAM_Fluid_Solver updateSimObject() called.");
	LOG(object->getName() << " id: " << object->getObjectId());

	SIM_SopScalarField* surface;
	UT_Vector3			divisions;
	surface = SIM_DATA_CAST(object->getNamedSubData("surface"), SIM_SopScalarField);
	divisions = surface->getDivisions();
	
	SIM_RawField *field = surface->getField();
	for(int x = 0; x < divisions.x(); x++){
		for(int y = 0; y < divisions.y(); y++){
			for(int z = 0; z < divisions.z(); z++){
				field->setCellValue(x, y, z, -1.0f);
			}
		}
	}

	LOG("Done.");
	LOG_UNDENT;
	return true;
}
	
SIM_Solver::SIM_Result
SIM_PhysBAM_Fluid_Solver::solveObjectsSubclass ( SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep){
	LOG_INDENT;
	LOG("SIM_PhysBAM_Fluid_Solver solveObjectsSubclass() called.");		
	bool new_world = false;
	worlddata = SIM_DATA_GET(*this, "PhysBAM_World", SIM_PhysBAM_WorldData);
	if(!worlddata){
		LOG("SIM_PhysBAM_Fluid_Solver solveObjectsSubclass() creating new PhysBAM_World data.");
		new_world = true;		
		worlddata = SIM_DATA_CREATE(*this, "PhysBAM_World", SIM_PhysBAM_WorldData, SIM_DATA_RETURN_EXISTING);
		if(!worlddata){
			LOG("SIM_PhysBAM_Fluid_Solver solveObjectsSubclass() unable to get PhysBAM_World data.");
			LOG_UNDENT;
			return SIM_Solver::SIM_SOLVER_FAIL;
		}
	}
	
	const SIM_Time 			curr_time = engine.getSimulationTime();
	
	/// Loop through new objects and add them into sim.
	if (newobjects.entries() > 0) {
		LOG("SIM_PhysBAM_Fluid_Solver solveObjectsSubclass() setting up new objects in sim:");	
		for( int i = 0; i < newobjects.entries(); i++ ){	
	    	if(!setupNewSimObject(0 , newobjects(i), curr_time)){
				LOG("SIM_PhysBAM_Fluid_Solver solveObjectsSubclass() unable set up new object:" << newobjects(i)->getName());
				LOG_UNDENT;
				return SIM_Solver::SIM_SOLVER_FAIL;
			}
		}
		/// exit with success. we don't want to run actual simulation here
		return SIM_Solver::SIM_SOLVER_SUCCESS;	
	}
		

	/// Update all the objects
	if (objects.entries() > 0){
		LOG("SIM_PhysBAM_Fluid_Solver solveObjectsSubclass() updating objects in sim:");
		for( int i = 0; i < objects.entries(); i++ ){ 
	    	if(!updateSimObject(0, objects(i))){
				LOG("SIM_PhysBAM_Fluid_Solver solveObjectsSubclass() unable update object:" << objects(i)->getName());
				LOG_UNDENT;
				return SIM_Solver::SIM_SOLVER_FAIL;
			}  
		}	
	}

	LOG_UNDENT;	
	LOG("Done");		
	return SIM_Solver::SIM_SOLVER_SUCCESS;
}	

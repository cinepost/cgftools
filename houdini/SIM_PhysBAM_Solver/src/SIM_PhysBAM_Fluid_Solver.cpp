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

bool SIM_PhysBAM_Fluid_Solver::setupNewSimObject(SIM_Object* object, SIM_Time time){
	LOG_INDENT;
	LOG("SIM_PhysBAM_Fluid_Solver solveObjectsSubclass() called.");
	LOG(object->getName() << " id: " << object->getObjectId());

	worlddata->addNewFluidObject(object, time);

	LOG("Done.");
	LOG_UNDENT;
	return true;
}
bool SIM_PhysBAM_Fluid_Solver::updateSimObject(SIM_Object* object){
	LOG_INDENT;
	LOG("SIM_PhysBAM_Fluid_Solver updateSimObject() called.");
	LOG(object->getName() << " id: " << object->getObjectId());

	unsigned char type = 0;
	physbam_simulation* sim = NULL;
	SIM_SopScalarField* primary = NULL;
	SIM_SopScalarField* density = NULL;
	SIM_SopScalarField* surface = NULL;
	
	density = SIM_DATA_CAST(object->getNamedSubData("density"), SIM_SopScalarField);
	surface = SIM_DATA_CAST(object->getNamedSubData("surface"), SIM_SopScalarField);
	
	UT_Vector3	divisions;
	
	if(density){
		type = SMOKE_TYPE;
		sim = worlddata->getSimulation(object->getObjectId(), SMOKE_TYPE);
		primary = density;
	}else if(surface){
		type = WATER_TYPE;
		sim = worlddata->getSimulation(object->getObjectId(), WATER_TYPE);
		primary = surface;
	}else{
		LOG("SIM_PhysBAM_Fluid_Solver updateSimObject() Unknown fluid object type !!! Skipping ...");
		LOG_UNDENT;	
		return false;
	}
	
	divisions = primary->getDivisions();	
	
	data_exchange::fluid_grid_data grid_data;
    ir.get_fluid_grid_data(sim, grid_data);
    
    int w = divisions.x();
	int h = divisions.y();
	int d = divisions.z();
    
    LOG("Updating from grid data " << w << " " << h << " " << d);  
	
	std::vector<float> densities ( w * h * d, 0.0f);
	ir.get_float_array(sim, ir.get_id(sim, "smoke_density"), &densities[0], densities.size(), 0);
	
	SIM_RawField *field = primary->getField();	
	
	for(int x = 0; x < w; x++){
		for(int y = 0; y < h; y++){
			for(int z = 0; z < d; z++){
				field->setCellValue(x, y, z, densities[z + w*y + h*w*x]);
				//field->setCellValue(x, y, z, field->getCellValue(x, y, z) + 0.1);
			}
		}
	}

	LOG("Done.");
	LOG_UNDENT;
	return true;
}
	
SIM_Solver::SIM_Result
SIM_PhysBAM_Fluid_Solver::solveSingleObjectSubclass(SIM_Engine& engine, SIM_Object& object, SIM_ObjectArray& feedback_to_objects, const SIM_Time& time_step, bool object_is_new){
	LOG_INDENT;
	LOG("SIM_PhysBAM_Fluid_Solver solveSingleObjectSubclass() called.");	
	physbam_simulation* 	sim = NULL;
	const SIM_Time 	curr_time = engine.getSimulationTime();

	LOG("SIM_PhysBAM_Fluid_Solver solveSingleObjectSubclass() creator: " << object.getCreatorNode()->getNetName());	

	worlddata = SIM_DATA_CREATE(*this, "PhysBAM_World", SIM_PhysBAM_WorldData, SIM_DATA_RETURN_EXISTING);
	if(!worlddata){
		LOG("SIM_PhysBAM_Fluid_Solver solveSingleObjectSubclass() unable to get PhysBAM_World data.");
		LOG_UNDENT;
		return SIM_Solver::SIM_SOLVER_FAIL;
	}

	if(object_is_new){
		/// Add new fluid object into simulation
		LOG("SIM_PhysBAM_Fluid_Solver solveObjectsSubclass() setting up new object in sim: " << sim);
		if(!setupNewSimObject(&object, curr_time)){
			LOG("SIM_PhysBAM_Fluid_Solver solveSingleObjectSubclass() unable set up new object:" << object.getName());
			LOG_UNDENT;
			return SIM_Solver::SIM_SOLVER_FAIL;
		}
	}else{			
		/// Run the simulation for the given time_step
		sim = worlddata->getSimulation(object.getObjectId(), SMOKE_TYPE);
		LOG("SIM_PhysBAM_Deformable_Solver solveSingleObjectSubclass() running ir.simulate_frame with sim:" << sim << " and timestep: " << time_step << " for object: " << object.getObjectId());	
		ir.simulate_frame(sim, .1);
		LOG("SIM_PhysBAM_Deformable_Solver solveSingleObjectSubclass() simulated.");		
					
		LOG("SIM_PhysBAM_Fluid_Solver solveSingleObjectSubclass() updating objects in sim: " << sim);
		if(!updateSimObject(&object)){
			LOG("SIM_PhysBAM_Fluid_Solver solveSingleObjectSubclass() unable update object:" << object.getName());
			LOG_UNDENT;
			return SIM_Solver::SIM_SOLVER_FAIL;
		}  
	}

	LOG_UNDENT;	
	LOG("Done");		
	return SIM_Solver::SIM_SOLVER_SUCCESS;
}		

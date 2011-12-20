#include <UT/UT_DSOVersion.h>
#include "SIM_PhysBAM_Deformable_Solver.h"
#include "SIM_PhysBAM_WorldData.h"

#include "SIM_PhysBAM_Commons.h"
#include "wrapper_header.h"
interface_routines ir;

#define		DEBUG
#define 	PHYSLIB_FILENAME	"libPhysBAM_Wrapper.so"

bool   PB_LIB_READY	= false;

void
initializeSIM(void *){
	srand((unsigned)time(0));
	IMPLEMENT_DATAFACTORY(SIM_PhysBAM_Deformable_Solver);
    IMPLEMENT_DATAFACTORY(SIM_PhysBAM_WorldData);
    
    logutils_indentation = 0;
    void* handle = dlopen(PHYSLIB_FILENAME, RTLD_LAZY);
    if(!handle){
        const char *p = dlerror();
        printf("dlopen error %s. Will bypass actual simulation !!!\n", p);
    }else{
		if(!ir.init(handle))
		{
			printf("Failed to initialize interface routines.\n");
			exit(1);
		}
		
		PB_LIB_READY = true;
		
		ir.physbam_init();
		ir.call<void>("set_playback_log", "/home/max/main.log");
		//ir.call<int>("set_enable_cout_logging", 0);
	}
	std::cout << "initializeSIM called." << std::endl;
};

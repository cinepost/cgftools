#ifndef __HPI_functs__
#define __HPI_functs__

physbam_simulation* (*hpi_create_simulation)();
bool (*hpi_destroy_simulation)(physbam_simulation*);
physbam_object* (*hpi_add_object)(physbam_simulation*, const data_exchange::simulation_object*);
physbam_force* (*hpi_add_force)(physbam_simulation*, const data_exchange::force*);
bool (*hpi_apply_force_to_object)(physbam_object*, physbam_force*);
bool (*hpi_simulate_frame)(physbam_simulation*);
int (*hpi_get_id)(physbam_base * obj, const char * attribute);
void (*hpi_set_int)(physbam_base * obj, int id, int x);
int (*hpi_get_int)(const physbam_base * obj, int id);
void (*hpi_set_float)(physbam_base * obj, int id, float x);
float (*hpi_get_float)(const physbam_base * obj, int id);
void (*hpi_get_vf3_array)(const physbam_base * obj, int id, data_exchange::vf3 * x, int length, int start);

void (*hpi_enable_logging)(const char* file);
void (*hpi_finish_logging)();

#endif

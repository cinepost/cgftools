#include "HPI_Object.h"

HPI_Object::HPI_Object(){
	pb_object = NULL;
	uid = 0;
}

HPI_Object::~HPI_Object(){

}

physbam_object*
HPI_Object::getPhysbamObject(){
	return pb_object;
}

int
HPI_Object::getUid(){
	return uid;
}

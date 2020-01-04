
#include "swing_door.h"

Swing::Swing(float range_value, int time){
current_value=0;
range=range_value;
discrete_time=time;
current_time=0;
first_time=1;
range_way=INTEGER;
printf("The Swing object was correctly created\n");
}


Swing::Swing(float range_value,range_type tipe, int time) {printf("This constructor was not defined yet\n");}

exception Swing::check(float value){

	if(range_way==INTEGER){ 
	    if(first_time==1){
	    	current_value=value; 
	    	first_time=0;
	    	return EXE_WAIT;
	    }
	    else {
	    	current_time++;
			if(value>(current_value+range) || value<(current_value-range)){
             current_time=0;
             current_value=value;
             return EXE_OK;
			}
			if(current_time>=discrete_time) {
				current_time=0;
				current_value=value;
				return EXE_OK;
			}
			return EXE_WAIT;
		}

	}
	else printf("This swing door algorith mas not created yet\n");
	return EXE_ERR;
}

exception Swing::synchronization(float value) {
current_value=value;
current_time=0;
return EXE_OK;
}

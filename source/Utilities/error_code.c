#include "error_code.h"

static ERR_CODE s_error = ERR_NO;

ERR_CODE set_last_error(ERR_CODE error){
	s_error = error;
	return s_error;
}

ERR_CODE set_module_error(ERR_MODULE e_type,ERR_VALUE e_value){
	if(e_value){
		s_error = (((unsigned int)e_type)<<8)+(unsigned int)e_value;
		return s_error;
	}
	return ERR_NO;
}

ERR_CODE get_last_error(){
	ERR_CODE error = s_error;
	s_error = ERR_NO;
	return error;
}

void PRINT_ERROR(ERR_CODE error){
}

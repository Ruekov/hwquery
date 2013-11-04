/*
 * papif.c
 * 
 */


#include <papi.h>
#include <stdio.h>
#include <stdlib.h> 
#include <malloc.h>
#include "papif.h"

int retval = 0;
int EventSet __attribute__((aligned(0x10000))) = PAPI_NULL;

void initialize_papi (int eventCode) {
  /* Initialize the PAPI library */
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error! %d\n",PAPI_EINVAL);
    handle_error(1);
  }
  
  if (EventSet==PAPI_NULL) {
  	if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
	  	printf("PAPI create eventset error\n");
  	  handle_error(1);
  	}
  } else {
  	if (PAPI_cleanup_eventset(EventSet) != PAPI_OK) {
  		printf("PAPI cleanup error\n");
			handle_error(1);
		}
	}
	
  if (PAPI_add_event(EventSet, eventCode) != PAPI_OK) {
		printf("PAPI add event error: %x\n", eventCode);
    handle_error(1);
  }  
}


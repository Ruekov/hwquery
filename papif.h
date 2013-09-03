/*
 * papif.h
 * 
 */
 
#ifndef PAPIF_H
#define PAPIF_H

#include <papi.h>
#include <stdio.h>
#include <stdlib.h> 
#include <malloc.h>

extern int retval;
extern int EventSet;
long_long values[1];

void initialize_papi(int eventCode);

static inline void start_papi() {
  /* Start counting events in the Event Set */
  if (PAPI_start(EventSet) != PAPI_OK) {
    handle_error(1);
  }
}

static inline long_long stop_papi() {
  /* Stop the counting of events in the Event Set */
  if (PAPI_stop(EventSet, values) != PAPI_OK) {
    handle_error(1);
  }
  if (values[0]!=0) {
    return values[0];
  }
}

#endif



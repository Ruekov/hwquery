/**
* @file test_cache_phi.c
* @author Guillem Rueda Cebollero
* @date 2012-2013
*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <papif.h>

#define samples 100000


#define CLEAN_MAINMEMORY_N {\
	for(i=0;i < memory_size;i++){ \
	mem[i] = 10000+i; \
	}\
	}
			
#define CLEAN_CACHE_N {\
		for(i=0;i < num_sets-1;i++){ \
		mem[i*num_sets*block_size] = extra_value+1+i; \
		}\
		}

#define PRINT_CACHE_N {\
		printf("Vector phi_clear: "); \
		for(i=0;i < num_sets-1;i++){ \
		printf(" %d ",mem[i*num_sets*block_size]); \
		}\
		printf("\n"); \
		}
		
#define SHOW_PHI {\
		for(i=0; i < size; i++){ \
		printf(" %d ",phi[i]); \
		}\
		printf(" \n"); \
		}
		
#define PRINT_REQ {\
		printf("[");\
		for(i=0; i < size; i++){ \
		printf(" %i ",hits[i]); \
		}\
		printf("]\n"); \
		}

#define PRINT_RES {\
		printf("[");\
		for(i=0; i < size; i++){ \
		printf(" %i ",access_test[i]); \
		}\
		printf("]\n"); \
		}

// global variables


unsigned char *mem = NULL;
int memory_size;

// functions

void hwquery(int addr[], int hit[], size_t size, int assoc, int blocksz, int cacheSize, int memorySize);
int test_access(int phi[], size_t size, int n_samples, int numsets, int blocksz, int extra_value);
	
int main(int argc, char **argv)
{
	
	if(argc < 6){
		
		printf("Arguments missing");
		return 0;
		
		}
	
	int assoc      = atoi(argv[1]);
	int blocksz    = atoi(argv[2]);
	int cacheSize  = atoi(argv[3]);
	int memorySize = atoi(argv[4]);
	int size       = atoi(argv[5]);
	
	int i;
	int access_test[size];
	
	for(i=0;i < size;i++){
		
		access_test[i] = atoi(argv[i+6]);
		
		}
	
	int hits[size];
	
	hwquery(access_test,hits,size,assoc,blocksz,cacheSize,memorySize);
	
	printf("\n");
	
	//for(i = 0; i < 10; i++){
	//	printf("Vector query: %i - Hit or Miss: %i \n",access_test[i],hits[i]);	
	//	}
	
	
	PRINT_RES;
	PRINT_REQ;
	
	return 0;
}

void hwquery(int addr[], int hit[], size_t size, int assoc, int blocksz, int cacheSize, int memorySize){
	
	int i, step;
	
	int associativity = assoc;
	int block_size = blocksz;
	int cache_size = cacheSize;
	int memory_size = memorySize;
	int num_sets;
	int extra_value;
	
	int highest_value = 0;
	
	for (i = 0; i < size; i++){
		if (addr[i] > highest_value){
			highest_value = addr[i];
			}
		}
	
	extra_value = highest_value + 1;
	

	//********************************************************
	//	setting mem for acces to the cache
	//********************************************************	
	
	num_sets = cache_size/(block_size*associativity);

	mem = malloc(memory_size); //sizeof(*mem) //(extra_value+associativity+1)*block_size*num_sets;
	
	if (NULL == mem) {
		printf("Error with malloc");
		exit(0);
    }
    
    //CLEAN_MAINMEMORY_N;
    CLEAN_CACHE_N;
	
	// first element is always a miss
	
	hit[0] = 0;
	
	printf(" 1 => MISS \n");
	
	for (step = 2; step <= size; step++){

		//********************************************************
		//	setting phi_hit
		//********************************************************
		
		int phi_hit[step];
			
		for (i = 0; i < step-1; i++){
			phi_hit[i] = addr[i];
			}
		
		phi_hit[step-1] = phi_hit[step-2];

		//********************************************************
		//	setting phi_miss
		//********************************************************

		int phi_miss[step];

		for (i = 0; i < step-1; i++){
				phi_miss[i] = addr[i];
				}
		
		phi_miss[step-1] = extra_value;
		
		//********************************************************
		//	setting phi_test
		//********************************************************
		
		int phi_test[step];
			
		for (i = 0; i < step; i++){
			phi_test[i] = addr[i];
			}	

		//********************************************************
		//	Doing samples
		//********************************************************	
	
		int rphi_hit = 0; 
		int rphi_miss = 0;
		int rphi_test = 0;
		
		printf(" TEST Phi_HIT (step: %d): ",step);
		
		rphi_hit  = test_access(phi_hit, step, samples, num_sets, block_size, extra_value);
		
		printf("TEST Phi_MISS (step: %d): ",step);
		
		rphi_miss = test_access(phi_miss, step, samples, num_sets, block_size, extra_value);
		
		printf("TEST Phi_TEST (step: %d): ",step);
		
		rphi_test = test_access(phi_test, step, samples, num_sets, block_size, extra_value);
			
		//********************************************************
		//	Is it a hit/miss?
		//********************************************************
		
		if(abs(rphi_hit-rphi_test) < abs(rphi_miss-rphi_test)){
			
			hit[step-1] = 1;
			printf("%i => HIT: Hit %i, Miss %i, Test %i Prom: %i < %i \n",step,rphi_hit, rphi_miss, rphi_test,abs(rphi_hit-rphi_test),abs(rphi_miss-rphi_test));
			
			}else{
				
			hit[step-1] = 0;	
			printf("%i => MISS: Hit %i, Miss %i, Test %i Prom: %i > %i \n",step,rphi_hit, rphi_miss, rphi_test,abs(rphi_hit-rphi_test),abs(rphi_miss-rphi_test));
	
				}
	
	}
	
	PRINT_CACHE_N;
	
	free(mem);
	mem = NULL;
	
}

int test_access(int phi[], size_t size, int n_samples, int numsets, int blocksz, int extra_value){
	
	initialize_papi(PAPI_L1_DCM);
	
	int num_miss = 0;
	register int x = 0;
	register int i; 
	register int j;
	register int num_sets = numsets;
	register int block_size = blocksz;
	
	SHOW_PHI;

	for ( j = 0; j < n_samples; j++){
		
		CLEAN_CACHE_N;

		/* BEGIN CRITICAL LOOP */
		
		start_papi();	// start miss counter
		
		for (i = 0; i < size; i++){
			
			x += mem[(phi[i])*num_sets*block_size];
			
			}
			
		num_miss = num_miss + stop_papi(); 	// stop miss counter
		
		/* END CRITICAL LOOP */
		
	}		
	
	printf(" ",x);
	//printf(" ACCESS X: %d  NUM MISS: %d \n",x, num_miss);

	return num_miss;
	
	}

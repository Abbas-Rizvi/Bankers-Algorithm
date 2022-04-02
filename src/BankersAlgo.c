/*
 ============================================================================
 Name        : bankers_algorithm_(Lab-4).c
 Author      : !YUNG VAMP!
 Version     :
 Copyright   : FREE DRAC
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#define NUM_CUSTOMERS 5
#define NUM_RESOURCES 5

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t mutex;

///////////////////////
//global vars
//available amount of each resource
int available[NUM_RESOURCES];

// maximum demand of each customer
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];

// amount currently allocated to each customer
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];

// remaining need of each customer
int need[NUM_CUSTOMERS][NUM_RESOURCES];



// isSafe
// Checks if a request is safe
// Returns 1 if valid and -1 if invalid
int isSafe(int customer){

	int valid = 1;

	// loop through all resources for customer
	for (int i = 0; i < NUM_RESOURCES; i++){

		// check if enough resource is available
		if (available[i] <= need[customer][i]){

			// if not enough resource available mark process as invalid
			valid = -1;
			break;

		}

	}

	return valid;

}

////////////////////////////
// Calculate Need
// receives customer number
// calculates customer need for all resources
// returns 1 if all needs are met, otherwise 0
////////////////////////////
int calcNeed(int customerNumber){


	// store number of needs that are met
	int fullfilledNeeds =0;

	// for each resource:
	for (int i =0; i < NUM_RESOURCES; i++){

		// need = maximum - allocated
		need[customerNumber][i] = (maximum[customerNumber][i] - allocation[customerNumber][i]);

		// if all the resources needs are met
		if (need[customerNumber][i] == 0)
			fullfilledNeeds += 1;

	}

	// if all needs are met
	// Sum of all needs met = number of needs
	if (fullfilledNeeds == NUM_RESOURCES)
		return 1;

	//if all needs are not met
	return 0;

}

////////////////////////////
// Manage Customers
// randomly generates requests to allocate
// randomly generates requests to release
////////////////////////////
void* manageCustomers(void* args){

	//convert passed customer_num to int
	//convert thread arg to int
	int *inpPtr = (int*)args;
	int customerNum = inpPtr;


	int request[NUM_RESOURCES];

	// inf loop
	while(1){


		// generate a request for allocating
		for (int i =0; i < NUM_RESOURCES; i++){

			// random amount of resource from 0 to need
			request[i] = rand() % need[customerNum][i];

		}

		// send request for allocation
		if (requestResources(customerNum,request) == 0){
			printf("Request by %d was Successful!", customerNum);
		} else{
			printf("Request by %d has Failed!", customerNum);
		}



		// generate a request for releasing
		for (int i =0; i < NUM_RESOURCES; i++){

			// random amount of resource from 0 to amount currently allocated for customer
			request[i] = rand() % need[customerNum][i];

		}

		//send request for releasing
		releaseResources(customerNum, request);
	}

	return NULL;

}

////////////////////////////
// Request Resources
// receives request and checks validity
// if request is safe, request resources
// if requested return 0, otherwise -1
////////////////////////////
int requestResources(int customerNum, int request[]){

	//lock semaphore to avoid conflicts
	int sem_wait(sem_t *sem);

	//check if it is safe
	if(isSafe(customerNum) == 1){


		// request resources
		for (int i =0 ; i < NUM_RESOURCES; i++){

			// remove appropriate amount of resource from available
			available[i] -= request[i];

			//decrease amount allocated for customer
			allocation[customerNum][i] -= request[i];

			// recalculate need
			calcNeed(customerNum);

		}

		// release semaphore
		int sem_post(sem_t *sem);

		// return 0 for successful request
		return 0;


	}

	//if not safe
	//return fail
	return -1;

}

////////////////////////////
// Release Resources
// receives request
// release resource from allocated
// if requested return 0
////////////////////////////
int releaseResources(int customerNum, int request[]){

	//lock semaphore to avoid conflicts
	int sem_wait(sem_t *sem);


	// request resources
	for (int i =0 ; i < NUM_RESOURCES; i++){

		// reallocate requested amount of each resource
		available[i] += request[i];

		//increase amount allocated for customer
		allocation[customerNum][i] += request[i];

		// recalculate need
		calcNeed(customerNum);

	}


	printf("Release by %d was Successful!", customerNum);

	// release semaphore
	int sem_post(sem_t *sem);

	// return 0 for successful request
	return 0;


}


///////////////////////
// printCustomer
///////////////////////
void printCustomer(int customer_num){

	printf("Resources for Customer %d\n", customer_num);

	for (int i=0; i< NUM_RESOURCES; i++ )
		printf("\t%c: %d\n",(int)'A' + i, allocation[customer_num][i]);

}


////generate a new request
//void genRequest(int customer_num){
//
//	int r;
//
//	for (int i =0; i < NUM_RESOURCES; i++){
//
//		// random number up to specified max
//		r = rand() % maximum[customer_num][i];
//
//		// change allocated resource to value
//		allocation[NUM_CUSTOMERS][i] = r;
//
//	}
//
//	printf("Random Resources Allocated!\n");
//	printCustomer(customer_num);
//
//}



///////////////////////
// Main
///////////////////////
int main(int argc, char* argv[]) {

	//end ptr
	char *ptr;


	// initialize random with time (randomness)
	srand(time(NULL));

	//check if there are appropriate number of args
	if (argc != ( NUM_RESOURCES + 1)){
		printf("Invalid number of resources available\nPlease Pass 5 Arguments");
		return -1;
	}

	long int parse;
	//initialize available vector with arg values
	for (int i =0; i< NUM_RESOURCES; i++){
		parse = strtol(argv[i],&ptr,10);

		available[i] = parse;
	}

	printf("\nNumber of Customers is %d", NUM_CUSTOMERS);
	printf("\nNumber of Resources is %d", NUM_RESOURCES);

	// initialize allocation values
	// remove value from available
	for(int i=0; i< NUM_CUSTOMERS; i++){

		printf("\n\nCUSTOMER ALLOCATION %d:\n",i+1);

		for (int j =0; j< NUM_RESOURCES; j++){

			// initialization
			printf("%c: ",(int)'A'+j);
			scanf("%d",&allocation[i][j]);

			// allocation
			available[j] -= allocation[i][j];

		}

	}


	//initialize max values
	for(int i=0; i< NUM_CUSTOMERS; i++){

		printf("\n\nCUSTOMER MAX VALUES %d:\n",i+1);

		for (int j =0; j< NUM_RESOURCES; j++){

			// initialization
			printf("%c: ",(int)'A'+j);
			scanf("%d",&maximum[i][j]);




		}

	}

	// allocate customer threads
	pthread_t threads[NUM_CUSTOMERS];

	// semaphore initialization
	sem_init(&mutex, 0, 1);



	//create threads to send customer arrays to
	for (int i=0; i < NUM_CUSTOMERS ; i++){

		pthread_create(&threads[i], NULL, manageCustomers, (void*)i);

	}


	//join threads
	for(int i = 0; i < NUM_CUSTOMERS; i++){
		pthread_join(threads[i],NULL);
	}

	// destroy semaphore
	sem_destroy(&mutex);


	return EXIT_SUCCESS;
}

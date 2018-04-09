#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <vector>

using namespace std;

sem_t semOne;
sem_t semTwo;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//global variables
int num_producing;
int num_consuming;
int buf_size;
int num_items;

//producer id and sleep time
typedef struct{
	int id_items;//store index
	int pause;//amount to sleep on 

}it_num;

vector<it_num*> vect_item;
 
//print function for struct 
void print(it_num* num){
	cout << num->id_items << " " << endl;
}


//producer 
void *proRun(void *arg) {
	int i = (int) arg;
	it_num *num;
	//cout << "producing " << endl;

	//dividing the threads evenly
	int start = i * (num_items/num_producing);
	int end = (i+1) * (num_items/num_producing);
	//cout << "Producing" << endl;
	for(int n = start; n < end; n++){
		//allocating memory for struct
		num = (it_num*) malloc(sizeof(it_num));
        
		//if buffer is full producer waits for available sapce
		if(buf_size == vect_item.size()){
			sem_wait(&semOne);	
		}
		//store index in id from struct 
		num->id_items = n;
		//random sleep time 
		num->pause = rand()% 900+200;
        
		pthread_mutex_lock(&lock);
		vect_item.push_back(num);
		pthread_mutex_unlock(&lock);
		//sleep(rand() % 300 + 1000);
		sem_post(&semTwo);
	}
	//freeStruct(num);	
}

//consumer
void *conRun(void *arg) {
	int n = (int) arg;
	it_num *num;

	//cout << "Consuming" << endl;
	//consuming items form buffer 
	//pop the last item on the vector
	//printing the consumer number and id 
	//if no items are found then consumer will wait 
 	
	while(1){
		sem_wait(&semTwo);
		//cout << " after Consuming" << endl;
		pthread_mutex_lock(&lock);
		num = vect_item.back();
		vect_item.pop_back();
	
		pthread_mutex_unlock(&lock);	
			
		cout << n << ":";
		cout << " Consuming ";
		print(num);	
		free(num);
		//sleep time form struct
		usleep(num->pause);
		sem_post(&semOne);
	}
	//freeStruct(num);
	//delete num;
}

int main(int argc, char **argv) {
    
	if (argc != 5){
		fprintf(stderr, "usage: ./hw1 num_prod num_cons buf_size num_items\n");
		exit(1);
	}
	sem_init(&semTwo, 0, 0);
	sem_init(&semOne, 0, buf_size);
	pthread_mutex_init(&lock, NULL);

	
	int i;

	pthread_t *threadsC;
	pthread_t *threadsP;

	threadsC = (pthread_t *) malloc(sizeof(pthread_t)*num_consuming);
	threadsP = (pthread_t *) malloc(sizeof(pthread_t)*num_producing);

	int e;//error for creating

	//creating producer threads 
	for (i = 0; i < num_producing; i++){
		e = pthread_create(&threadsP[i], NULL, proRun, (void *) i);
		if(e != 0){
			fprintf(stderr, "Could not create thread");
			exit(1);
		}
	}
	//creating consumer threads	
	for (i = 0; i < num_consuming; i++){
	   	e = pthread_create(&threadsC[i], NULL, conRun, (void *) i);
		if(e != 0){
			fprintf(stderr, "Could not create thread");
			exit(1);
		}
    }
	int err;//for joining

	//joining producer threads 	 
	for(i = 0; i < num_producing; i++){
		err = pthread_join(threadsP[i], NULL);
		if(err != 0){
			fprintf(stderr, "Could not join producer threads");
			exit(1);
		}
	}
	fprintf(stderr, "Done producing!\n");
    
	//joining consumer threads 
	for(i = 0; i < num_consuming; i++){

		err = pthread_join(threadsC[i], NULL);
		if(err != 0){
			fprintf(stderr, "Could not join Consumer threads");
			exit(1);
		}
	}
	free(threadsP);
	free(threadsC);
}

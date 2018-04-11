//
//  main.cpp
//  OSII
//
//  Created by Kyleen Gonzalez on 4/5/18.
//  Copyright © 2018 Kyleen Gonzalez. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <vector>
#include <iostream>
#include <typeinfo>
#include <sstream>

using namespace std;


//global variables
int numberProducer;
int numberConsumer;
int bufferSize;
int numberItems;

//creating semaphores
sem_t  spaceAvailable;
sem_t  addItemForConsumption;


//mutex mtx;
pthread_mutex_t locked = PTHREAD_MUTEX_INITIALIZER;
/*
 Items must contain two numbers:
 - An Identification number (from 0 to num-items - 1)
 - A random sleep time (between 200 - 900 usecs)
 */
typedef struct{
    long long idNumbers;
    int sleepTime;
} createdItems;


//Shared global buffer that stores items, created from producer and taken from consumer
vector<createdItems*> vector_items;

/*
 void assign global variables that have been filtered and converted into chars from command line arguments
 */
void assignCommandLineArguments(int value, int i) {
    switch (i)
    {
        case 1:
            numberProducer = value;
            break;
        case 2:
            numberConsumer = value;
            break;
        case 3:
            bufferSize = value;
            break;
        case 4:
            numberItems = value;
            break;
        default: // code to be executed if n doesn't match any cases
            fprintf(stderr, "Error assigning values");
            exit(1);
    }
}
/*
 Takes command line arguments (strings) and converts them to intengers.
 Then checks to make sure they are greater then zero.
 If value is less than or equal to zero program exits with error statement.
 Then send converted String to int values and index to assigning function
 */
void checkValuesGreaterThanZero(string value, int index) {
    
    //int StringToInt = stoi(value);
    stringstream val(value);
    int StringToInt = 0;
    val >> StringToInt;
    
    if(StringToInt <= 0){
        fprintf(stderr, "Values must be > 0\n");
        exit(1);
    }else {
        assignCommandLineArguments(StringToInt, index);
    }
}


/*
 - Produce items and store them in vector
 - Before producing each item, producer must sleep random time(200-700 usec)
 - If buffer full, producer must wait
 */
void *producerProducingItems(void *threadid) {
    long divideItemsThreadStart;
    long divideItemsThreadEnd;
    long tid;
    
    //Capture thread id that was passed from pthread_Create
    tid = (long)threadid;
    
    //Allocating memory for typedef struct
    createdItems *item =(createdItems*)malloc(sizeof(createdItems));
    
    //To evenly divide items between Threads
    divideItemsThreadStart = tid * (numberItems/ numberProducer);
    divideItemsThreadEnd = ((tid + 1) * (numberItems / numberProducer));
    
    //    cout <<"Start: " << divideItemsThreadStart << endl ;
    //    cout <<"End: " << divideItemsThreadEnd << endl;
    
    /*
     
     */
    sem_wait(&spaceAvailable);//empty was initilied to buffserSize will go on to put item in vectory
    pthread_mutex_lock(&locked);
    for (long i = divideItemsThreadStart; i < divideItemsThreadEnd; i++) {
        //Go To SLeeeeeeeeeeep
        int sleepValue = rand()%(700 - 300)+ 300;
        usleep(sleepValue);
        
        //Producer first waits for a buffer to become empty in order to put data into it.
        //Consumer waits for a buffer to become filled before using it.
        //Creating the item (id 0 - (numItems - 1) && Random sleep time 200 - 900 usecs)
        item->idNumbers = i;
        item->sleepTime = rand()%(900 - 200)+ 200;
        vector_items.push_back(item);
        
        // if there are 1 or more threads waiting, wake 1
        //this will set full semaphore to 1 thus waking the consumer thread
        sem_post(&addItemForConsumption);
        //cout <<"Vector size: " << vector_items.size() <<endl;
        
    }
    pthread_mutex_unlock(&locked);
    pthread_exit(NULL);
}

/*
 - Consume items from the shared global buffer
 - On Consumption, consumer will sleep and print the Consumer number and ID of item to stdout
 Introduce two semaphores empty and full. Which the threads will use to indicate when an item entry has been emptied or filled.
 */
void *consumerConsumingItems(void *threadid) {
    //Grabing the passed argument from thread creating and storing this will be Consumer Number
    long tid = (long) threadid;
    createdItems item;
    
    while(1) {
        
        sem_wait(&addItemForConsumption); //if a consumer thread runs first semaphore intilized to 0, the call will block the consumer and wait semaphore in producer to post
        pthread_mutex_lock(&locked);
        //cout << "Vector size inside mutex and semaphore: " << vector_items.size() << endl;
        item = *vector_items.back(); //Grabing the last item of the vector
        int sleep = item.sleepTime;
        cout << tid << ":" << " Consuming " << item.idNumbers << endl;
        //free(item);//After remove item from vector we free the memory
        vector_items.pop_back(); //Removes the last element in the vector, effectively reducing the container size by one.
        
        sem_post(&spaceAvailable);
        
        pthread_mutex_unlock(&locked);
        
        usleep(sleep); //go to sleep....
        
    }
}


int main(int argc, const char * argv[]) {
    int rc;
    if (argc != 5){
        fprintf(stderr, "usage: ./hw1 num_prod num_cons buf_size num_items\n");
        exit(1);
    }
    // Cycle through command line arguments.
    //Call function to check values and assign arguments to global variables.
    for (int i = 1; i < argc; i++) {
        checkValuesGreaterThanZero(argv[i], i);
    }
    
    //Pass it buffersize this will allow for producer threads access and add items to buffer
    sem_init(&spaceAvailable, 0, 0);
    sem_init(&spaceAvailable, 0, bufferSize);
    pthread_mutex_init(&locked, NULL);
//    if ((spaceAvailable = sem_open("available_sem", O_CREAT, 0644, bufferSize)) == SEM_FAILED) {
//        perror("semaphore initilization");
//        exit(1);
//    }
//    if ((spaceAvailable = sem_open("addit_sem", O_CREAT, 0644, 0)) == SEM_FAILED) {
//        perror("semaphore initilization");
//        exit(1);
//    }
    
    //Need to have a unique Thread ID for each thread
    //Create mulptiple thread ids build an array of thread ids (dataType name[sizeOfThread])
    pthread_t tidsProducer[numberProducer];
    pthread_t tidsConsumer[numberConsumer];
    
    
    //Creating PRODUCER THREADS
    //To create thread, must pass in ThreadID to know which thread you are working on
    //Attributes control how thread is going to function, pass address of attributs: Using NULL
    //When passing in function that must execute do not put brackets, because it will return a value (no brackets function pointer)
    //Pass in arguments with Thread so pass address of variable.
    for(int i = 0; i < numberProducer; i++){
        rc = pthread_create(&tidsProducer[i], NULL, producerProducingItems, (void *)(size_t) i);
        if (rc) {
            cout << "Error: Unable to create thread, " << rc << endl;
            exit(-1);
        }
    }
    
    //Creating CONSUMER THREADS
    for(int i = 0; i < numberConsumer; i++){
        //Creating Multiple Threads
        pthread_create(&tidsConsumer[i], NULL, consumerConsumingItems, (void *)(size_t) i);
    }
    
    //Want main thread to wait until thread has done its work
    for (int i = 0; i < numberProducer; i++) {
        pthread_join(tidsProducer[i], NULL);
    }
    
    //After all producer threads have finished, print message to stderr
    fprintf(stderr, "Done producing!!\n");
    
    
    for (int i = 0; i < numberProducer; i++) {
        pthread_join(tidsConsumer[i], NULL);
    }
    pthread_exit(NULL);
    
    return 0;
}

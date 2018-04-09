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

using namespace std;

//for testing
int counter = 0;

//global variables
int numberProducer;
int numberConsumer;
int bufferSize;
int numberItems;

//creating semaphores
//http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2008/Notes/threads-semaphores.txt
sem_t  *empty;
sem_t  *full;
sem_t  * mux;


//mutex lock1;
/*
 Items must contain two numbers:
 - An Identification number (from 0 to num-items - 1)
 - A random sleep time (between 200 - 900 usecs)
 */
typedef struct{
    long long idNumbers;
    int sleepTime;
} createdItems;

void printItem(createdItems i) {
    cout <<"ID: " << i.idNumbers <<" " << "Sleep Time: " << i.sleepTime <<endl;
}


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
            //cout << "NumberProducer " << numberProducer <<"\n";
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
    
    int StringToInt = stoi(value);
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
void *producerProducingItems(void *arg) {
    long long divideItemsThreadStart;
    long long divideItemsThreadEnd;
    int itemsDividedbyProducers = numberItems/numberProducer;
    
    //Not use mutex lock and unlock directly in case exception is thrown. RAII
    //Whenever the guard goes out of cscope mutex will unlock
    
    //Allocating memory for typedef strucks
    createdItems *item =(createdItems*)malloc(sizeof(createdItems));
    
    //Capture thread value that was passed
    //Had to use long long (Error: Cast from pointer to smaller type 'int' loses information)
    long long proNumber = (long long) arg;
    
    /*
        TODO: Way to evenly divide number of items between number of threads.
        Grabbing the thread ID or Number of Thread we multiply it by number of items
        For end we take start number and add it to number of items  - 1 to account for 0
     */
    divideItemsThreadStart = proNumber * itemsDividedbyProducers;
    divideItemsThreadEnd = divideItemsThreadStart + numberItems;
    /*
        - num_items must be evenly divided between num_producers
        - if num_items = 1000 and num_producers = 10, then each producer thread
        - produce 100 unique items
        - - Using a for loop to create items
     */
    for (long long i = divideItemsThreadStart ; i < divideItemsThreadEnd; i++) {
//        int sleepValue = rand()%(700 - 300)+ 300;
//        usleep(sleepValue);
        
        
        /*
             int sem_wait(sem_t *s) {
             wait until value of semaphore s is greater than 0
             decrement the value of semaphore s by 1
             }
         
             int sem_post(sem_t *s) {
             increment the value of semaphore s by 1
             if there are 1 or more threads waiting, wake 1
             }
             */
        cout <<"Inside producer function" << endl;
        //Creating the item (id 0 - (numItems - 1) && Random sleep time 200 - 900 usecs)
        item->idNumbers = i;
        item->sleepTime = rand()%(900 - 200)+ 200;
        
//        if (bufferSize == vector_items.size()) {
//            //sem_wait(&empty);
//        }
        //Producer first waits for a buffer to become empty in order to put data into it.
        //Consumer waits for a buffer to become filled before using it.
        sem_wait(empty);//empty was initilied to zero will go on to put item in vectory
        sem_wait(mux);//add mutex, to add more locking mechanism
        cout <<"Should execute right away, both empy and mux are 1 " << endl;
        vector_items.push_back(item);
        sem_post(mux);
        sem_post(full);//this will set full semaphore to 1 thus waking the consumer thread
        
      //cout <<"Vector item ID" << "[" << i <<"]: " << vector_items[i]->idNumbers <<endl;
//        cout <<"Vector item SleepTime"<< "[" << i <<"]: "  << vector_items[i]->sleepTime <<endl;
    
    }

    //Way to finish thread and 0 because not passing anything
    //If wanted to pass, value will be cause on pthread_join(threadID, value);
    pthread_exit(0);
}

/*
 - Consume items from the shared global buffer
 - On Consumption, consumer will sleep and print the Consumer number and ID of item to stdout
 - If no items in buffer, consumer must wait for items.
 
 Introduce two semaphores empty and full. Which the threads will use to indicate when an item entry has been emptied or filled.
 
 */
void *consumerConsumingItems(void *arg) {
    
    //Grabing the passed argument from thread creating and storing this will be Consumer Number
     long long consumerNumber = (long long) arg;
    createdItems *item =(createdItems*)malloc(sizeof(createdItems));
    
    while(1) {
        sem_wait(full); //if consumer runs first full was intilized to 0, the call will block the consumer and wait for another thread to call
        //semp_post on the semaphore
        sem_wait(mux);
        cout <<"Why isn't it waiting.. " << endl;
        //Creating stuct item
        item = vector_items.back(); //Grabing the last item of the vector
        vector_items.pop_back(); //Removes the last element in the vector, effectively reducing the container size by one.
        sem_post(mux);
        sem_post(empty);
        cout << consumerNumber << ":" << " Consuming " << item->idNumbers;
        usleep(item->sleepTime);
    
    }
}


int main(int argc, const char * argv[]) {
    if (argc != 5){
        fprintf(stderr, "usage: ./hw1 num_prod num_cons buf_size num_items\n");
        exit(1);
    }
    // Cycle through command line arguments.
    //Call function to check values and assign arguments to global variables.
    for (int i = 1; i < argc; i++) {
        checkValuesGreaterThanZero(argv[i], i);
    }
    if ((mux = sem_open("muxSem", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("semaphore initilization");
        exit(1);
    }
    if ((empty = sem_open("emptySem", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("semaphore initilization");
        exit(1);
    }
    if ((full = sem_open("full", O_CREAT, 0644, 0)) == SEM_FAILED) {
        perror("semaphore initilization");
        exit(1);
    }
    //sem_init(&empty, 0, 1);
    //sem_init(&full, 0, 0);
    //sem_init(&mux, 0, 1);
    
    //Need to have a unique Thread ID for each thread
    //Create mulptiple thread ids build an array of thread ids (dataType name[sizeOfThread])
    pthread_t tidsProducer[numberProducer];
    pthread_t tidsConsumer[numberConsumer];
    
    
    //Creating PRODUCER THREADS
    //To create thread, must pass in ThreadID to know which thread you are working on
    //Attributes control how thread is going to function, pass address of attributs
    //When passing in function that must execute do not put brackets, because it will return a value (no brackets function pointer)
    //Pass in arguments with Thread so pass address of variable.
    for(int i = 0; i < numberProducer; i++){
        //creating attributes
        pthread_attr_t attrProducer;
        pthread_attr_init(&attrProducer);
        //Creating Multiple Threads
        pthread_create(&tidsProducer[i], &attrProducer, producerProducingItems, (void *)(size_t) i);
    }
    
    //Creating CONSUMER THREADS
    for(int i = 0; i < numberConsumer; i++){
        //creating attributes
        pthread_attr_t attrConsumer;
        pthread_attr_init(&attrConsumer);
        //Creating Multiple Threads
        pthread_create(&tidsConsumer[i], &attrConsumer, consumerConsumingItems, (void *)(size_t) i);
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
    
    
//    cout << "Vector at [0] " <<vector_items[0]->idNumbers <<endl;
//    cout << "Vector at [1] " <<vector_items[1]->idNumbers <<endl;
    
    return 0;
}

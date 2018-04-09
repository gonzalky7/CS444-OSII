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
sem_t  empty;
sem_t  full;


mutex lock1;
/*
 Items must contain two numbers:
 - An Identification number (from 0 to num-items - 1)
 - A random sleep time (between 200 - 900 usecs)
 */
typedef struct{
    int idNumbers;
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
    //Not use mutex lock and unlock directly in case exception is thrown. RAII
    //Whenever the guard goes out of cscope mutex will unlock
    
    //Allocating memory for typedef strucks
    createdItems *item =(createdItems*)malloc(sizeof(createdItems));
    
    //Capture thread value that was passed
    //Had to use long long (Error: Cast from pointer to smaller type 'int' loses information)
    long long passedValue = (long long) arg;
    cout <<"PAssedValue: " <<passedValue<<endl;
    
    
    /*
        TODO: Way to evenly divide number of items between number of threads.
     */
    
    /*
        - num_items must be evenly divided between num_producers
        - if num_items = 1000 and num_producers = 10, then each producer thread
        - produce 100 unique items
        - - Using a for loop to create items
     */
    for (int i = 0; i < numberItems; i++) {
        int sleepValue = rand()%(700 - 300)+ 300;
        usleep(sleepValue);
        
        
        //Creating the item (id 0 - (numItems - 1) && Random sleep time 200 - 900 usecs)
        item->idNumbers = i;
        item->sleepTime = rand()%(900 - 200)+ 200;
        
        //Producer first waits for a buffer to become empty in order to put data into it.
        //Consumer wiats for a buffer to become filled before using it.
        sem_wait(&empty);
        lock_guard<mutex> guard(lock1);
        vector_items.push_back(item);
        lock1.unlock();
        sem_post(&full);
        
        
//        cout <<"Vector item ID" << "[" << i <<"]: " << vector_items[i]->idNumbers <<endl;
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
 */
void *consumerConsumingItems(void *arg) {
    
    //Grabing the passed argument from thread creating and storing this will be Consumer Number
     long long consumerNumber = (long long) arg;
    //Creating stuct item
    createdItems *item =(createdItems*)malloc(sizeof(createdItems));
    
    while(1) {
    
        sem_wait(&full);
        lock_guard<mutex> guard(lock1);
        item = vector_items.back();
        //.back() returns a referce to the last element in the vector
        item = vector_items.back();
        lock1.unlock();
        cout << consumerNumber << ":" << " Consuming " << item->idNumbers;
        usleep(item->sleepTime);
        free(item);
        sem_post(&empty);
        
    }
    
    //Using a while loop for consumer to
//    while(1){
//        //sem_wait(&semTwo);
//
//        //Lock this critical section
//        lock_guard<mutex> guard(lock1);
//        //.back() returns a referce to the last element in the vector
//        item = vector_items.back();
//        //.pop_back() removes the last element in the vector, effectivey reducing the container size by one.
//        vector_items.pop_back();
//        lock1.unlock();
//
//        cout << consumerNumber << ":" << " Consuming " << item->idNumbers;
//        free(item);
//
//        //sleep time form struct
//        usleep(item->sleepTime);
//
//       // sem_post(&semOne);
//    }
    
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

    sem_init(&empty, 0, bufferSize);
    sem_init(&full, 0, 0);
    
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

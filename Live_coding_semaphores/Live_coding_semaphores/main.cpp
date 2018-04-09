//
//  main.cpp
//  Live_coding_semaphores
//
//  Created by Kyleen Gonzalez on 4/5/18.
//  Copyright © 2018 Kyleen Gonzalez. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h> //for usleep

using namespace std;

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

sem_t  my_semaphore;

void *consume(void *arg) {
    int n = 0;
    while(1) {
        //wait until value of semaphore is greater than 0
        //decrement the value of semaphore s by 1
        // critical section here
        printf("consuming # %d..\n", n++);
        sem_post(&my_semaphore);
    }
    return NULL;
}

int main(int argc, const char * argv[]) {

    sem_init(&my_semaphore, 0, 0);
    
    pthread_t consumer;
    int i = 0;


    pthread_create(&consumer, NULL, consume, (void *) (size_t)i);

    for (i = 0; i < 100; i++) {
        sem_wait(&my_semaphore);
        usleep(10 * (rand() % 3));
        printf("Producing #%d\n", i);
        //increment the value of semaphore  by 1
        //if there are 1 or more threads waiting, wake 1
    }

    printf("done!\n");

    return 0;
}


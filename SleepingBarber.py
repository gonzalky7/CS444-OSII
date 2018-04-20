#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import threading
import time
from random import *
import datetime

globalAverageWait = 0
totalHairCuts = 0
globClientsLeft = 0
lock = threading.Lock()
cv = threading.Condition(lock)
clientsDoneFlag = False
avgWaitTimeClients = 0
avgSleepTimeBarbers = 0

#creating Semaphore for barbers available
barbers_available_sem = threading.Semaphore()
#creating Semaphore for chairs available
chairs_available_sem = threading.Semaphore()

def incrementGlobal():
    global totalHairCuts
    totalHairCuts += 1
    return totalHairCuts


def incrementGlobalClientsLeft():
    global globClientsLeft
    globClientsLeft += 1
    return globClientsLeft


#For the barber function we pass the thread ID's, number of clients and haircut time
#The barber uses conditional variabls for signaling
#Waiting for clients to arrive and to be awakened. Woked? Woken up?
def Barbers(threadID, num_c, hairCutTime):
    global barbers_available_sem, totalHairCuts,globClientsLeft,clientsDoneFlag
    global avgSleepTimeBarbers
    #The barber must acquire the condition(and thus the related lock), and can then attempt to cut hair
    #The barbers continue in a while loop and waiting to be signaled by the clients
    while True:
        cv.acquire()
        print "barber ",threadID, ": sleeping..."
        start_time = time.time()
        cv.wait() #Barbers waits or sleeps until client signals
        elapsed_time = time.time() - start_time
        avgSleepTimeBarbers = elapsed_time
        #Barber has been signaled continue#
        #I had to use this flag here bc when we signaled from main the barber threads were hanging when I put the return function
        #This just keeps the barber from incrementing the global counts and printing
        if not clientsDoneFlag:
            incrementGlobal()
            print "barber ",threadID, ": haircut..."
        time.sleep(float(hairCutTime)/1000.) #SLeep for  amount of time for haircut
        cv.release()
        barbers_available_sem.release()#increments the counter bc babrber is available

        #We use a boolean flag that will return when all the clients have finished getting a hair cut
        #This flag is set after all the clients have joined
        if (clientsDoneFlag):
            return

#For client we pass in thread ID and the haircut time which each client has to wait to
#In the client function we have a semaphore that does not block, so if no barbers available the thread will go to the else statement
#Where the client will come to another semaphore for available chairs in the waiting rooom.
#Again we have a no blocking semaphore if no chairs availble client leaves
def Clients(threadID, hairCutTime):
    global chairs_available_sem
    global barbers_available_sem
    global avgWaitTimeClients
    #will arrive at random intervals and signal barber
    #if barbers available returns true then clients can proceed, else they check the waiting room
    
    if barbers_available_sem.acquire(blocking=False):#decrements the counter
        print "client ", threadID, ": arriving..."
        cv.acquire()#The client thread needs to acquire the condition before it can notify the barber
        cv.notify()#signal that client is ready
        print "client ", threadID,": haircut..."
        time.sleep(float(hairCutTime)/1000.)
        cv.release()
    else:
        if chairs_available_sem.acquire(blocking=False):#we do a try wait becuase we already checked if barbers were available
            print "client ", threadID,": waiting..."
            #To calculate average we get the current time waiting
            #Then we acquire the current time when client has been barber availble: subtracting the two times and storing in global variable
            start_time = time.time()
            barbers_available_sem.acquire()#check if barber is available
            elapsed_time = time.time() - start_time
            avgWaitTimeClients = elapsed_time
            cv.acquire()
            cv.notify()#signal that client is ready
            print "client ", threadID,": haircut..."
            time.sleep(float(hairCutTime)/1000.)
            cv.release()
            chairs_available_sem.release()#increments
        else:
            print "client ", threadID,": leaving..."
            incrementGlobalClientsLeft()#capturing the number of clients that left
    
def main():
    global totalHairCuts, globClientsLeft,chairs_available_sem, barbers_available_sem,clientsDoneFlag
    global avgWaitTimeClients
    
    #capturing all the command line arguments and some error checking
    try:
        num_barbers = sys.argv[1]
        num_clients = sys.argv[2]
        num_chairs = sys.argv[3]
        #maximum time between clients
        arrival_t = sys.argv[4]
        #required time for haircut
        hair_cut_t = sys.argv[5]
    except ValueError:
        print 'Value must be greater than 0'
        sys.exit(1)
    except IndexError:
        print 'usage: python SleepingBarber.py num_barbers num_clients num_chairs arrival_t haircut_t'
        sys.exit(1)  # abort execution


    ##########CREATING THREADS FOR BARBERS AND CLIENTS##########
    #creating Semaphore for barbers available
    barbers_available_sem = threading.Semaphore(value=int(num_barbers))
    #creating Semaphore for chairs available
    chairs_available_sem = threading.Semaphore(value=int(num_chairs))

    barbersThread = []
    for i in range(int(num_barbers)):
        barbers = threading.Thread(target=Barbers, args=(i, num_clients,hair_cut_t))
        barbersThread.append(barbers)
        barbers.start()

    clientsThreads = []
    for i in range(int(num_clients)):
        #Clients are created at random times, thus arrivcing at random times to the barber
        x = randint(1, int(arrival_t))
        time.sleep(float(x)/1000.)
        clients = threading.Thread(target=Clients, args=(i,hair_cut_t))
        clientsThreads.append(clients)
        clients.start()

    threadJoinClients = 0
    for thread in clientsThreads:
        threadJoinClients += 1
        thread.join()
    #Give some time for barbers
    time.sleep(5)
    ####Notify the barbers to go home##


    if int(threadJoinClients) == int(num_clients):
        clientsDoneFlag = True #set a flag so in barbers function, barbers know to return
        cv.acquire()
        cv.notifyAll()#notify all the barbers that it is good to go home
        cv.release()

    threadJoinBarbers = 0
    for thread in barbersThread:
        threadJoinBarbers += 1
        thread.join()

    if int(threadJoinBarbers) == int(num_barbers):
        print "\n"
        print "TOTALS:"
        print "Total haircuts: ",totalHairCuts
        print "Avg Barber sleep time:",float(avgSleepTimeBarbers)/float(num_barbers)
        print "Number clients that Left:",globClientsLeft
        print "Avg client wait time: ",float(avgWaitTimeClients)/float(num_clients)

if __name__ == '__main__':
    main()

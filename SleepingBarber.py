#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import threading
import time
from random import *


globvar = 0
globClientsLeft = 0
lock = threading.Lock()
cv = threading.Condition(lock)


#creating Semaphore for barbers available
barbers_available_sem = threading.BoundedSemaphore()
#creating Semaphore for chairs available
chairs_available_sem = threading.BoundedSemaphore()

def incrementGlobal():
    global globvar
    globvar += 1
    return globvar


def incrementGlobalClientsLeft():
    global globClientsLeft
    globClientsLeft += 1
    return globClientsLeft

#Barber will be in a loop, waitinf for signal and semaphore
def Barbers(threadID, num_c, hairCutTime):
    global barbers_available_sem
    global globvar
    print "Num_c: ", num_c
    #The barber must acquire the condition(and thus the related lock), and can then attempt to cut hair?
    while True:
        cv.acquire()
        print "barber ",threadID, ": sleeping..."
        
        cv.wait() #Barbers waits or sleeps until client signals
        #Barber has been signaled continue#
        incrementGlobal()
        print "Glob var: ", globvar
        print "barber ",threadID, ": haircut..."
        time.sleep(int(hairCutTime)) #SLeep for  amount of time for haircut
        cv.release()
        barbers_available_sem.release()#increments the counter bc babrber is available

        ###GOO HOME ###
        if int(num_c) == int(globvar):
            cv.acquire()
            cv.wait()
            cv.release()
            return

#When barber haircuting haircut_t time r equired for a haircut
def Clients(threadID, hairCutTime):
    global chairs_available_sem
    #will arrive at random intervals and signal barber
    #if barbers available returns true then clients can proceed, else they check the waiting room
    
    if barbers_available_sem.acquire(blocking=False):#decrements the counter
        print "client ", threadID, ": arriving..."
        cv.acquire()#The client thread needs to acquire the condition before it can notify the barber
        cv.notify()#signal that client is ready
        print "client ", threadID,": haircut..."
        time.sleep(int(hairCutTime))
        cv.release()
    else:
        if chairs_available_sem.acquire():#we block or continue if chair available
            print "client ", threadID,": waiting..."
            barbers_available_sem.acquire(blocking=False)#we block and decrement
            cv.acquire()
            cv.notify()#signal that client is ready
            print "client ", threadID,": haircut..."
            time.sleep(int(hairCutTime))
            cv.release()
            chairs_available_sem.release()#increments
        else:
            print "client ", threadID,": leaving..."
            incrementGlobalClientsLeft()
            return
    
def main():
    global globvar
    global globClientsLeft
    
    try:
        #//1 barber, 1 client, 1 chair, arrival_t = 100, haircut_t = 10 μs
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
    barbers_available_sem = threading.BoundedSemaphore(value=int(num_barbers))
    #creating Semaphore for chairs available
    chairs_available_sem = threading.BoundedSemaphore(value=int(num_chairs))


    for i in range(int(num_barbers)):
        barbers = threading.Thread(target=Barbers, args=(i, num_clients,hair_cut_t))
        barbers.start()

    clientsThreads = []
    for i in range(int(num_clients)):
        #Clients are created at random times, thus arrivcing at random times to the barber
        x = randint(1, int(arrival_t))
        time.sleep(int(x))
        clients = threading.Thread(target=Clients, args=(i,hair_cut_t))
        clientsThreads.append(clients)
        clients.start()


    for thread in clientsThreads:
        thread.join()

    time.sleep(5)

    ####Notify the barbers to go home##
    cv.acquire()
    cv.notify_all()#notify all the barbers that it is good to go home
    cv.release()

    print "TOTALS:\n"
    print "Total haircuts: ",globvar
    print "Avg Barber sleep time:"
    print "Number clients that Left:",globClientsLeft
    print "Avg Client wait time: "

if __name__ == '__main__':
    main()

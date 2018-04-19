#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import threading
import time
from random import *

lock = threading.Lock()
cv = threading.Condition(lock)

#creating Semaphore for barbers available
barbers_available_sem = threading.BoundedSemaphore()
#creating Semaphore for chairs available
chairs_available_sem = threading.BoundedSemaphore()

#Barber will be in a loop, waitinf for signal and semaphore
def Barbers(threadID, num_c, hairCutTime):
    hairCut = hairCutTime
    clients_count = int(num_c)
    
    #print "Clients_count type: ", type(clients_count), "\n"
    #The barber must acquire the condition(and thus the related lock), and can then attempt to cut hair?
    while True:
        if clients_count == 0:
            print "no more clients:", clients_count
            sys.exit(1)
        
       
        if clients_count > 0:
            cv.acquire()
            print "barber ",threadID, ": sleeping..."
            cv.wait() #Barbers waits or sleeps until client signals
            #Barber has been signaled continue#
            print "barber ",threadID, ": haircut..."
            time.sleep(int(hairCut)) #SLeep for  amount of time for haircut
            clients_count -= 1
            print "Client counts: ",clients_count
            cv.release()
            print "condition variable release"
            barbers_available_sem.release()#increments the counter
            print "Barbers available release"

#When barber haircuting haircut_t time r equired for a haircut
def Clients(threadID, hairCutTime):
    #will arrive at random intervals and signal barber
    hairCut = hairCutTime
    #if barbers available returns true then clients can proceed, else they check the waiting room
    
    
    if barbers_available_sem.acquire(blocking=False):#decrements the counter
        print "client ", threadID, ": arriving..."
        cv.acquire()#The client thread needs to acquire the condition before it can notify the barber
        cv.notify()#signal that client is ready
        print "client ", threadID,": haircut..."
        time.sleep(int(hairCut))
        cv.release()
    else:
        print "client ", threadID,": waiting..."
        if chairs_available_sem.acquire():
            cv.acquire()
            cv.notify()#signal that client is ready
            print "client ", threadID,": haircut..."
            time.sleep(int(hairCut))
            cv.release()
            chairs_available_sem.release()
        else:
            print "client ", threadID,": leaving..."

    
def main():
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

    for i in range(int(num_clients)):
        #Clients are created at random times, thus arrivcing at random times to the barber
        x = randint(1, int(arrival_t))
        time.sleep(int(x))
        clients = threading.Thread(target=Clients, args=(i,hair_cut_t))
        clients.start()

   
    

if __name__ == '__main__':
    main()

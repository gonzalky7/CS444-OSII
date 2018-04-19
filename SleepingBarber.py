#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import threading
import time
from random import *

lock = threading.Lock()
cv = threading.Condition(lock)
barbers_available_sem = threading.BoundedSemaphore()



#Barber will be in a loop, waitinf for signal and semaphore
def Barbers(threadID, num_c):
    clients_count = int(num_c)
    print "Clients_count type: ", type(clients_count), "\n"
    #The barber must acquire the condition(and thus the related lock), and can then attempt to cut hair?
    
    while True:
        cv.acquire()
        cv.wait() #Wait until client notifies
        if clients_count > 0:
            print "Cutting hair", threadID, "\n "
            clients_count -= 1
            cv.release()
        elif clients_count == 0:
            print "no more clients \n"
            sys.exit(1)

#When barber haircuting haircut_t time r equired for a haircut

def Clients(threadID):
    #will arrive at random intervals and signal barber
    
    barbers_available_sem.acquire()#decrements the counter See if barber available if not block
    #The client thread needs to acuire the condition before it can notify the barber
    cv.acquire()
    print "Clients ", threadID, "\n"
    cv.notify()#signal that client is ready
    cv.release()
    barbers_available_sem.release()#increments the counter


    
def main():
    try:
        #//1 barber, 1 client, 1 chair, arrival_t = 100, haircut_t = 10 μs
        #Semaphore buffer size for number of barbers
        num_barbers = sys.argv[1]
        print "num_barbers " + num_barbers
        num_clients = sys.argv[2]
        print "num_clients " + num_clients
        #Semaphore buffer size for number of chairs
        num_chairs = sys.argv[3]
        print "num_chairs " + num_chairs
        #maximum time between clients
        arrival_t = sys.argv[4]
        print "arrival_t " + num_chairs
        #required time for haircut
        hair_cut_t = sys.argv[5]
        print "hair_Cut " + hair_cut_t
    except ValueError:
        print 'Value must be greater than 0'
        sys.exit(1)
    except IndexError:
        print 'usage: python SleepingBarber.py num_barbers num_clients num_chairs arrival_t haircut_t'
        sys.exit(1)  # abort execution



        #####CREATING THREADS FOR BARBERS AND CLIENTS####
    int(num_barbers)
    #creating Semaphore for barbers available
    barbers_available_sem = threading.BoundedSemaphore(value=num_barbers)
    #creating Semaphore for chairs available
#    chairs_available_sem = threading.BoundedSemaphore(value=num_chairs)

    for i in range(int(num_barbers)):
        barbers = threading.Thread(target=Barbers, args=(i, num_clients))
        barbers.start()


    for i in range(int(num_clients)):
        #Clients are created at random times, thus arrivcing at random times to the barber
        x = randint(1, int(arrival_t))
        time.sleep(int(x))
        clients = threading.Thread(target=Clients, args=(i,))
        clients.start()

   
    

if __name__ == '__main__':
    main()

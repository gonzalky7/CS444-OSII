#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import threading
import time
from random import *

lock = threading.Lock()

cv = threading.Condition(lock)

#Barber will be in a loop, waitinf for signal and semaphore
def Barbers(arg):
    print "Barber ", arg, "\n "
    
def Chairs(arg):
    print "Chairs ", arg, "\n"
    
    
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


    for i in range(int(num_chairs)):
        chairs = threading.Thread(target=Chairs, args=(i,))
        chairs.start()

#    for i in range(int(num_barbers)):
#        barbers = threading.Thread(target=Barbers, args=(i,))
#        barbers.start()


    for i in range(int(num_clients)):
        #Clients are created at random times, thus arrivcing at random times to the barber
        x = randint(1, int(arrival_t)) # Pick a random number between 1 and arrival_t given
        time.sleep(int(x))
        clients = threading.Thread(target=Barbers, args=(i,))
        clients.start()


#    #clients use wait condition variables to signal a barber
#    clients = threading.Thread(target=barbers, args=(1,))
#    clients.start()

   
    

if __name__ == '__main__':
    main()

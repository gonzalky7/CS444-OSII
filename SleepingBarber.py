#!/usr/bin/python

import sys
import threading
from time import sleep

cv = threading.Condition()
lock = threading.Lock()

#Note: the notify() and notifyAll() methods doesn't release the lock; this means that the thread 
#or threads awakened will not return from their wait() call immediately, 
#but only when the thread that called notify() or notifyAll() finally relinquishes ownership of the lock.

def ping(arg):
    #The acquire() method locks the
    #Lock and blocks execution until the release() method in some other coroutine sets it to unlocked
    print "Before ping function acquire lock\n"
    cv.acquire()
    lock.acquire()#lock
    try:
        print "ping!"
        print "After ping function acquire lock\n"
    finally:
        lock.release()#unlock
        print "ping function release\n"
    cv.wait()
    cv.notify()
    print "ping function notify\n"
    
def pong(arg):
    print "Before pong acquire lock\n"
    lock.acquire()#Acquire the underlying lock. This method calls the
    #corresponding method on the underlying lock; the return value is whatever that method returns.
    cv.acquire()
    try:
        print "PONG!\n"
        cv.notify()#wakes up one of the threads waiting for the condition variabel, if any waiting.
        print "Thread notified in pong\n"
    finally:
        lock.release()
        cv.release()
        print "Released in pong\n"
    
def main():
    #print 'Number of arguments:', len(sys.argv), 'arguments.'
    #print 'Argument List:', str(sys.argv)
    #
    
    pings = threading.Thread(target=ping, args=(1,))
    pings.start()
    
    pongs = threading.Thread(target=pong, args=(1,))
    pongs.start()



    
if __name__ == '__main__':
    main()

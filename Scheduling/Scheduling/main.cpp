//
//  main.cpp
//  Scheduling
//
//  Created by Kyleen Gonzalez on 4/24/18.
//  Copyright © 2018 Kyleen Gonzalez. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <map>
#include <iterator>
#include <list>

using namespace std;
string algorithm;
int sim_time;

float avg_wait_time = 0.0;
int through_put = 0;
int remaining_processes = 0;

typedef struct Processes{
    int p_id;
    int arival_time;
    int burst_time;
} PROCESS;

list<Processes> process_list;

void checkArgs(const char * arg[], int argc) {
    const char * algos[3] = {"FCFS","SJF", "RR"};
    // Makre sure simulation time is greater than 0
    if (atoi(arg[1]) <= 0) {
        fprintf(stderr, "Simulation time must be greater than 0\n");
        exit(1);
    }
    // Round robin has a time slice quanta
    if (strcmp(arg[2], algos[2]) == 0 && argc == 3) {
        fprintf(stderr, "usage: ./hw4 simulation_time RR [time_slice]\n");
        exit(1);
    }else if (strcmp(arg[2], algos[2]) == 0 && argc == 4){
        // Make sure [time slice] is greater than 0
        if (atoi(arg[3]) <= 0) {
            fprintf(stderr, "Time slice must be greater than 0\n");
            exit(1);
        }
    }
    //Make sure correct algorithm was passed
    for (int i = 0; i < 3; i++) {
        if(strcmp(arg[2], algos[i]) == 0){
            //Inside here there was a match with one of the algorithms
            //Make sure only 2 arguments were passed for FCFS and SJF time slice is only for RR
            if (argc != 3 && strcmp(algos[i],"RR") != 0) {
                fprintf(stderr, "usage: ./hw4 simulation_time algorithm [time_slice RR ONLY]\n");
                exit(1);
            }else{
                algorithm = algos[i];
                return;
            }
        }
    }
    fprintf(stderr, "algorithm: FCFS, SJF, RR\n");
    exit(1);
}

bool compare_processes_arrival_time(Processes p1, Processes p2) {
    if(p1.arival_time > p2.arival_time){
        return false;
    }
    return true;
}

void printPerformanceStats(){
    fprintf(stderr, "Throughput =  %d\n", through_put);
    fprintf(stderr, "Avg wait time =  %.2f\n", avg_wait_time/through_put);
    fprintf(stderr, "Number of remaining processes =  %d\n", remaining_processes);
    exit(1);
}

void fcfs(list<Processes> process_list) {
    list<Processes>::iterator process_it;
    int simulation_time = sim_time;
    bool times_up = true;
    remaining_processes = int(process_list.size());
    //Need to sort processes by arrival time given random out of order arrival times fron .txt file
    process_list.sort(compare_processes_arrival_time);
    
    //start simulation time and the scheduling of processes
     while(times_up) {
         cout << "=================================" << endl;
        for (process_it = process_list.begin(); process_it != process_list.end(); process_it++) {
            int burst_time_timer = process_it->burst_time;
            int burst_time = process_it->burst_time;
            
            cout <<avg_wait_time<<": "<<"  scheduling "<< "PID: " << process_it->p_id<<" CPU =  " << burst_time<<endl;
            //DO WORK: This is simulating the process doing work
            while (burst_time_timer != 0) {
                burst_time_timer -= 1;
            }
            //Service time is just the addition of all burst times / processes
            avg_wait_time += burst_time;
            cout <<avg_wait_time<<": "<<"             "<< "PID: " << process_it->p_id<<" terminated" << endl;
            
            through_put++;//as a successful process has finished add it to throughput
            remaining_processes -= 1;//decrement list size to show remaining processes
            simulation_time -=1;
            //check on simulation time break out of the loop when time has run out
            if (simulation_time == 0) {
                times_up = false;
                break;
            }
            //If simulation time is higher then number of processes given
            if (remaining_processes == 0) {
                cout << "===================================" << endl;
                printPerformanceStats();
            }
        }
    }
    //Simulation Time has ran out
    cout << "=================================" << endl;
    cout << "Simulation time ran out" <<endl;
    printPerformanceStats();
}







int main(int argc, const char * argv[]) {
    // Make sure number of arguments have been entered : UNIX> ./hw4 sim_time algorithm [time_slice]
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "usage: ./hw4 simulation_time algorithm [time_slice RR ONLY]\n");
        exit(1);
    }else {
        checkArgs(argv,argc);
        sim_time = atoi(argv[1]);
        string algorithm = argv[2];
    }
    //Keep Reading in processes from stdin until end of file
    //int tmp;
    int process_id ;
    int arrival;
    int burst ;
    while (cin >> process_id >> arrival >> burst) {
        //Fill up list before we send it to algorithms
        //cin >> process_id >> arrival >> burst;
        PROCESS p;
        p.p_id = process_id;
        p.arival_time = arrival;
        p.burst_time = burst;
        //cout<<"Pushing id: " <<p.p_id<<endl;
        process_list.push_back(p);
    }
    
    if (algorithm == "FCFS"){
        fcfs(process_list);
    }
    
    return 0;
}

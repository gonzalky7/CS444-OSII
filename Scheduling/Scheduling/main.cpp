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
#include <numeric>
#include <vector>

using namespace std;
string algorithm;
int sim_time;
int time_slice = 0;

float avg_wait_time = 0;
float avg_turn_around_time = 0.0;

int through_put = 0;
int remaining_processes = 0;


int simulation_time = sim_time;
int i = 0;
int service_time = 0;


vector<int> wt;
vector<int> tat;
vector<int> ct;

typedef struct Processes{
    int p_id;
    int arrival_time;
    int burst_time;
    int completion_time;
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
    if(p1.arrival_time > p2.arrival_time){
        return false;
    }
    return true;
}

bool compare_processes_burst_time(Processes p1, Processes p2) {
    if(p1.burst_time > p2.burst_time){
        return false;
    }
    return true;
}

void printPerformanceStats(){
    fprintf(stderr, "Throughput =  %d\n", through_put);
    fprintf(stderr, "Avg wait time =  %.2f\n", avg_wait_time);
    fprintf(stderr, "Avg turnaround time =  %.2f\n",  avg_turn_around_time);
    fprintf(stderr, "Number of remaining processes =  %d\n", remaining_processes);
    exit(1);
}

void findAvgerages(vector<int> &wt,vector<int> &tat,int n, int t) {
    float sum_of_waits = 0.0;
    float sum_of_turn_around_times = 0.0;
    
    for (int i = 0; i < t; i++) {
         sum_of_turn_around_times += tat[i];
    }
    for (int i = 0; i < n; i++) {
        sum_of_waits += wt[i];
    }
    avg_wait_time = sum_of_waits/through_put;
    avg_turn_around_time = sum_of_turn_around_times/through_put;
    printPerformanceStats();
}

void fcfs(list<Processes> process_list) {
    list<Processes>::iterator process_it;
    remaining_processes = int(process_list.size());
    process_list.sort(compare_processes_arrival_time);
   
    //start simulation time and the scheduling of processes
         cout << "=================================" << endl;
        for (process_it = process_list.begin(); process_it != process_list.end(); process_it++) {
            int burst_time_timer = process_it->burst_time;
            
            cout <<service_time<<": "<<"  scheduling "<< " PID: " << process_it->p_id<<" CPU =  " << process_it->burst_time<<endl;
            
            while (burst_time_timer != 0) {
                burst_time_timer -= 1;
            }
            wt.push_back(service_time - process_it->arrival_time);
            
            cout <<service_time<<": "<<"              "<< "PID: " << process_it->p_id<<" terminated" << endl;
            tat.push_back(process_it->burst_time + wt[i]);
            service_time += process_it->burst_time;
            i++;
            through_put++;//as a successful process has finished add it to throughput
            remaining_processes -= 1;//decrement list size to show remaining processes
            
            //check on simulation time break out of the loop when time has run out
            simulation_time -= 1;
            if (simulation_time == 0) {
                //Simulation Time has ran out
                cout << "=================================" << endl;
                cout << "Simulation time ran out" <<endl;
                int n = int(wt.size());
                int t = int(tat.size());
                findAvgerages(wt,tat, n,t);
                break;
            }
            //If simulation time is higher then number of processes given
            if (remaining_processes == 0) {
                cout << "===================================" << endl;
                cout << "Processes have finished" <<endl;
                int n = int(wt.size());
                int t = int(tat.size());
                findAvgerages(wt,tat, n,t);
            }
        }
}


void sjf(list<Processes> process_list){
    process_list.sort(compare_processes_burst_time);
    list<Processes>::iterator process_it;
    remaining_processes = int(process_list.size());


    
    //start simulation time and the scheduling of processes
    cout << "=================================" << endl;
    for (process_it = process_list.begin(); process_it != process_list.end(); process_it++) {
        int burst_time_timer = process_it->burst_time;
        
        cout <<service_time<<": "<<"  scheduling "<< " PID: " << process_it->p_id<<" CPU =  " << process_it->burst_time<<endl;
        
        while (burst_time_timer != 0) {
            burst_time_timer -= 1;
        }
        
        wt.push_back(service_time - process_it->arrival_time);
        
        cout <<service_time<<": "<<"              "<< "PID: " << process_it->p_id<<" terminated" << endl;
        
        tat.push_back(process_it->burst_time + wt[i]);
        service_time += process_it->burst_time;
        i++; //incrementing spots in wait time
        through_put++;//as a successful process has finished add it to throughput
        remaining_processes -= 1;//decrement list size to show remaining processes
        
        //check on simulation time break out of the loop when time has run out
        simulation_time -= 1;
        if (simulation_time == 0) {
            //Simulation Time has ran out
            cout << "=================================" << endl;
            cout << "Simulation time ran out" <<endl;
            int n = int(wt.size());
            int t = int(tat.size());
            findAvgerages(wt,tat, n,t);
            break;
        }
        //If simulation time is higher then number of processes given
        if (remaining_processes == 0) {
            cout << "===================================" << endl;
            cout << "Processes have finished" <<endl;
            int n = int(wt.size());
            int t = int(tat.size());
            findAvgerages(wt,tat, n,t);
        }
    }
}

void rr(list<Processes> process_list) {
    list<Processes>::iterator process_it;
    process_list.sort(compare_processes_arrival_time);//List sorted by arrival time
    remaining_processes = int(process_list.size());
    vector<int> bt;
    int turn_around_time = 0;
    int wait_time = 0;
    /*  vector<int> wt;
          vector<int> tat;
          vector<int> ct; */
    
    process_it = process_list.begin();
    bt.push_back(process_it->burst_time);//Keep original burst times for calculation of wait time
    int i = 0;
    
    while(remaining_processes != 0 && process_it != process_list.end()) {
        
        if (process_it->burst_time > time_slice) {
            cout <<"Process burst time > time slice: "  <<endl;
            cout <<"Process ID: " <<process_it->p_id<< " burst time: " << process_it->burst_time <<endl;
            process_it->burst_time = process_it->burst_time - time_slice;
            service_time += time_slice;
            cout << "Service time: " << service_time <<endl;
        }else if (process_it->burst_time <= time_slice) {
            cout <<"Process burst time < time slice: " <<endl;
            cout <<"Process ID: " <<process_it->p_id<< " burst time: " << process_it->burst_time <<endl;
            service_time += process_it->burst_time;
            cout << "Service time: " << service_time <<endl;
            process_it->completion_time = service_time;//completion used for turn around time.
            turn_around_time = service_time - process_it->arrival_time;
            
            cout << "turn_around_time: " << turn_around_time<<endl;
            process_it->burst_time =  0;//process is done
            tat.push_back(turn_around_time); //only need to get the average of turn around time
            wait_time = turn_around_time - bt[i];
            cout << "Bt["<<i<<"]: "<<  bt[i] <<endl;
            wt.push_back(wait_time);
            cout <<"Wait time: " << wait_time <<endl;
            i++;
            remaining_processes -= 1;//decrement list size to show remaining processes
            cout << "Remaining processes: " << remaining_processes <<endl ;
        }
        if (process_it == process_list.end()) {
            process_it = process_list.begin();
            i = 0;
        }else if (process_it++->arrival_time <= service_time) {
            int temp = 0;
            temp = process_it++->arrival_time ;
            cout <<"Next process ID: " <<process_it->p_id<< " burst time: " << process_it->burst_time <<" arrival time" << temp <<endl;
            cout << "Service time: " << service_time <<endl;
            i++;
            //process_it++;
        }else {
            process_it = process_list.begin();
            //go back to beginning if next process has not arrived
            cout <<"Begin process ID: " <<process_it->p_id<< " burst time: " << process_it->burst_time <<endl;
            i = 0;
        }
    }
    
//    for (int i = 0; i < bt.size(); i++) {
//        cout << "Vector of burst times: " << bt[i] << endl;
//    }
  
    
//    for (process_it = process_list.begin(); process_it != process_list.end(); process_it++) {
//        cout << "Proccess: " << process_it->p_id << " arrival time: " << process_it->arrival_time <<
//        " burst time: " << process_it->burst_time << " completion time: " << process_it->completion_time <<endl;
//
//    }
}




int main(int argc, const char * argv[]) {
    // Make sure number of arguments have been entered : UNIX> ./hw4 sim_time algorithm [time_slice]
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "usage: ./hw4 simulation_time algorithm [time_slice RR ONLY]\n");
        exit(1);
    }else {
        checkArgs(argv,argc);
        sim_time = atoi(argv[1]);
        algorithm = argv[2];
        time_slice = atoi(argv[3]);
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
        p.arrival_time = arrival;
        p.burst_time = burst;
        process_list.push_back(p);
    }
    if (algorithm == "FCFS"){
        fcfs(process_list);
    }else if (algorithm == "SJF"){
        sjf(process_list);
    }else if (algorithm == "RR"){
        rr(process_list);
    }
    
    return 0;
}

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

using namespace std;
string algorithm;
int sim_time;

typedef struct Processes{
    int p_id;
    int arival_time;
    int burst_time;
} PROCESS;


void checkArgs(const char * arg[], int argc) {
    const char * algos[3] = {"FCFS","SJF", "RR"};
    for (int i = 0; i < 3; i++) {
        if(strcmp(arg[2], algos[i]) == 0){
            cout << "algos["<<i<<"]:"<<algos[i] <<endl;
            algorithm = algos[i];
            return;
        }
    }
    fprintf(stderr, "algorithm: FCFS, SJF, RR\n");
    exit(1);
}

void fcfs() {
    
  
   

}

int main(int argc, const char * argv[]) {
    // Make sure number of arguments have been entered : UNIX> ./hw4 sim_time algorithm [time_slice]
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "usage: ./hw4 simulation_time algorithm [time_slice]\n");
        exit(1);
    }else {
        checkArgs(argv,argc);
        //int sim_time = atoi(argv[1]);
        string algorithm = argv[2];
    }
    
    while (!cin.eof()) {
        int process_id ;
        int arrival;
        int burst ;
        
        cin >> process_id >> arrival >> burst;
        if (algorithm == "FCFS"){
            PROCESS pid;
            pid.p_id = process_id;
            pid.arival_time = arrival;
            pid.burst_time = burst;
            
            cout << "Procces id: "<<pid.p_id<<" arrival_Time: "<<pid.arival_time << " burst_Time: "<<pid.burst_time<<endl;
            //fcfs();
        }
    }
    
    
    return 0;
}

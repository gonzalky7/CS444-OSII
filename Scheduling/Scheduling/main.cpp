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

void fcfs() {
    
  
   

}

int main(int argc, const char * argv[]) {
    // Make sure number of arguments have been entered : UNIX> ./hw4 sim_time algorithm [time_slice]
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "usage: ./hw4 simulation_time algorithm [time_slice RR ONLY]\n");
        exit(1);
    }else {
        checkArgs(argv,argc);
        //int sim_time = atoi(argv[1]);
        string algorithm = argv[2];
    }
    //Keep Reading in processes from stdin until end of file
    while (!cin.eof()) {
        int process_id ;
        int arrival;
        int burst ;

        cin >> process_id >> arrival >> burst;
        if (algorithm == "FCFS"){
            PROCESS p;
            p.p_id = process_id;
            p.arival_time = arrival;
            p.burst_time = burst;

            cout << "Procces id: "<<p.p_id <<" arrival_Time: "<<p.arival_time << " burst_Time: "<<p.burst_time<<endl;
            //fcfs();
        }
    }
    
    return 0;
}

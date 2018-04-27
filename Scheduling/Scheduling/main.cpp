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
multimap<int, int> fcfs_multimap;
void fcfs(int pid, int arrival, int burst) {
    
  
    fcfs_multimap.insert(pair<int, int> (pid, burst));
    

}

int main(int argc, const char * argv[]) {
    //UNIX> ./hw4 sim_time algorithm [time_slice]

    
    string sim_time = argv[1];
    string algorithm = argv[2];
    
    string line;
    
//    int tmp;
    while (!cin.eof()) {
        int pid ;
        int arrival;
        int burst ;
        
        cin >> pid >> arrival >> burst;
        if (algorithm == "FCFS"){
            fcfs(pid, arrival, burst);
        }
    }
    multimap <int, int> :: iterator itr;
    cout << "\tPID\tBURST TIME\n";
    for (itr = fcfs_multimap.begin(); itr != fcfs_multimap.end(); ++itr)
    {
        cout  <<  '\t' << itr->first
        <<  '\t' << itr->second << '\n';
    }
    cout << endl;
    
    return 0;
}

//
//  main.cpp
//  Scheduling
//
//  Created by Kyleen Gonzalez on 4/24/18.
//  Copyright © 2018 Kyleen Gonzalez. All rights reserved.
//

#include <iostream>

using namespace std;

int main(int argc, const char * argv[]) {
    //UNIX> ./hw4 sim_time algorithm [time_slice]
    
    cout << "You have entered " << argc
    << " arguments:" << "\n";
    
    for (int i = 0; i < argc; ++i)
        cout << argv[i] << "\n";
    
    
    return 0;
}

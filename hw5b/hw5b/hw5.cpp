//
//  main.cpp
//  hw5b
//
//  Created by Kyleen Gonzalez on 5/23/18.
//  Copyright © 2018 Kyleen Gonzalez. All rights reserved.
//

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>    // std::find
#include <stdio.h>

using namespace std;

int num_frames;
string algorithm;
vector<int> page_vec;


void checkArgs(const char * arg[], int argc) {
    const char * algos[3] = {"FIFO","LRU", "OPT"};
    // Makre sure frame number is greater than 0
    if (atoi(arg[1]) <= 0) {
        fprintf(stderr, "Frame number must be greater than 0\n");
        exit(1);
    }
    
    //Make sure correct algorithm was passed
    for (int i = 0; i < 3; i++) {
        if(strcmp(arg[2], algos[i]) == 0){
            //Inside here there was a match with one of the algorithms
            algorithm = algos[i];
            return;
        }
    }
    fprintf(stderr, "algorithm: FIFO, LRU, OPT\n");
    exit(1);
}

int frame_index = 0;

void fifo( vector<int> p_vect) {
    int frames[num_frames];
    //Initializing frames
    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
    }
    bool page_not_found = false;
    int page_fault_counter = 0;
    while (!p_vect.empty()) {
        if (frame_index == num_frames) frame_index = 0;
        
        cout << "==============================="<<endl;
        cout <<"requesting page:       " <<p_vect[0] << "\n";
        
        for (int i = 0; i < num_frames; i++) {
            //if page found
            if (frames[i] == p_vect[0]) {
                cout <<"--------------------------------*"<<endl;
                //after putting reference page into physical memory pop off front
                p_vect.erase(p_vect.begin());

                for (int i = 0; i < num_frames; i++) {
                    cout << frames[i]<< endl;
                }
                cout <<"--------------------------------*"<<endl;
                //break out of the loop
                page_not_found = false;
                break;
            }
            //Page not found
            page_not_found = true;
        }
        
        if (page_not_found) {
            cout << "PAGE FAULT!!!\n";
            page_fault_counter++;
            frames[frame_index] = p_vect[0];
            p_vect.erase(p_vect.begin());
            cout <<"--------------------------------"<<endl;
            for (int i = 0; i < num_frames; i++) {
                cout << frames[i]<< endl;
            }
            cout <<"--------------------------------"<<endl;
            page_not_found = false;
            frame_index++;
        }
    }
    
    cout << "_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_" << endl;
    fprintf(stderr, "%d page faults occured",page_fault_counter);
}



void lru( vector<int> p_vect) {
    //we keep just adding p_vect into the first position of frames array
    //then move each element down one
    
    deque<int> frame_deq;
    deque<int>::iterator it;
    bool page_not_found = false;
    int page_fault_counter = 0;
    //initializing
    for (int i = 0; i < num_frames; i++) {
        frame_deq.push_back(-1);
    }
    
   
    while (!p_vect.empty()) {
        
        cout << "==============================="<<endl;
        cout <<"requesting page:       " <<p_vect[0] << "\n";
        
        for (int i = 0; i < num_frames; i++) {
            //if page found
            if (frame_deq[i] == p_vect[0]) {
                cout <<"--------------------------------"<<endl;
                it = find (frame_deq.begin(), frame_deq.end(), p_vect[0]);
                frame_deq.erase(it);
                //after putting reference page into physical memory pop off front
                frame_deq.push_front(p_vect[0]);
                p_vect.erase(p_vect.begin());
                
                for (int i = 0; i < num_frames; i++) {
                    cout << frame_deq[i]<< endl;
                }
                cout <<"--------------------------------"<<endl;
                //break out of the loop
                break;
            }
            //Page not found
            page_not_found = true;
        }
        
        if (page_not_found) {
            cout << "PAGE FAULT!!!\n";
            page_fault_counter++;
            frame_deq.push_front(p_vect[0]);
            frame_deq.pop_back();
            p_vect.erase(p_vect.begin());
            cout <<"--------------------------------"<<endl;
            for (int i = 0; i < num_frames; i++) {
                cout << frame_deq[i]<< endl;
            }
            cout <<"--------------------------------"<<endl;
        }

    }
    cout << "_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_" << endl;
    fprintf(stderr, "%d page faults occured",page_fault_counter);
}

void opt( vector<int> p_vect) {
    int frames[num_frames];
    vector <int>::iterator it = p_vect.begin ();
    
    //Initialize
    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
    }
    bool did_not_find_empty_space = false;
    bool page_not_found = false;
    int page_fault_counter = 0;
    bool pages_were_found = false;
    
    while (!p_vect.empty()) {
        cout << "==============================="<<endl;
        cout <<"requesting page:       " <<p_vect[0] << "\n";
        
        //First check is looking for empty spaces  in pysical memory represented by -1
        for (int i = 0; i < num_frames; i++) {
            //if page found
            if (frames[i] == -1) {
                cout << "PAGE FAULT" << endl;
                page_fault_counter++;
                cout <<"--------------------------------"<<endl;
                //after putting reference page into physical memory pop off front
                frames[i] = p_vect[0];
                
                p_vect.erase(p_vect.begin());
                
                for (int i = 0; i < num_frames; i++) {
                    cout << frames[i]<< endl;
                }
                cout <<"--------------------------------"<<endl;
                did_not_find_empty_space = false;
                //break out of the loop
                break;
            }
            did_not_find_empty_space = true;
        }
        
        if(did_not_find_empty_space){
            //Next check is seeing if the page is already in memory
            for (int i = 0; i < num_frames; i++) {
                //if page found
                if (frames[i] == p_vect[0]) {
                    //pop off front of reference string
                    p_vect.erase(p_vect.begin());

                    cout <<"--------------------------------"<<endl;
                    for (int i = 0; i < num_frames; i++) {
                        cout << frames[i]<< endl;
                    }
                    cout <<"--------------------------------"<<endl;
                    //break out of the loop
                    page_not_found = false;
                    did_not_find_empty_space = false;
                    break;
                }
                //Page not found
                page_not_found = true;
            }
        }
      
        //If there is no empty space (-1) and the page is not in the page table
        
        if (page_not_found) {
            cout << "PAGE FAULT!!!\n";
            page_fault_counter++;
            
            long index_tracker = 0;
            int frame_index = 0;
            
            for (int i = 0; i < num_frames; i++) {
                it = find (p_vect.begin(), p_vect.end(), frames[i]);
                
                if (it != p_vect.end()){
                    //found the page store values
                    if (index_tracker < (it - p_vect.begin()) + 1) {
                        //store new farther index
                        index_tracker = (it - p_vect.begin()) + 1;
                        //store the frame that has the farthest page reference
                        frame_index = i;
                        pages_were_found = true;
                    }
                }else {
                    //The element was not found so this means it has the farthest reference
                    //we replace the frame with value
                    frames[i] = p_vect[0];
                    //then we pop off the refence page
                    p_vect.erase(p_vect.begin());
                    cout <<"--------------------------------"<<endl;
                    for (int i = 0; i < num_frames; i++) {
                        cout << frames[i]<< endl;
                    }
                    cout <<"--------------------------------"<<endl;
                    //we have replaced the value go to next reference string
                    pages_were_found = false;
                    break;
                }
            }
            if (pages_were_found) {
                //if all pages were in p_vec then the farthest value should be stored in frame_index
                frames[frame_index] = p_vect[0];
                //then we pop off the refence page
                p_vect.erase(p_vect.begin());
                cout <<"--------------------------------"<<endl;
                for (int i = 0; i < num_frames; i++) {
                    cout << frames[i]<< endl;
                }
                cout <<"--------------------------------"<<endl;
                //we have replaced the value go to next reference string
                pages_were_found = false;
            }
        }
        
    }
    cout << "_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_" << endl;
    fprintf(stderr, "%d page faults occured",page_fault_counter);
}



int main(int argc, const char * argv[]) {
    
    if (argc != 3) {
        fprintf(stderr, "usage: ./hw5 num_frames algorithm\n");
        exit(1);
    }else {
        checkArgs(argv,argc);
        num_frames = atoi(argv[1]);
    }
    

    
    //Keep Reading in page refernce numbers from stdin until end of file
    int page ;
  
    while (cin >> page) {
        //Fill up list before we send it to algorithms
        page_vec.push_back(page);
    }
    if (algorithm == "FIFO"){
        fifo(page_vec);
    }else if (algorithm == "LRU"){
        lru(page_vec);
    }else if (algorithm == "OPT"){
        opt(page_vec);
    }
    
    return 0;
}

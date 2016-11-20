/*
 * Author: Matthias Hahn <matthias.hahn@intel.com>
 * Copyright (C) 2014 Intel Corporation
 * This file is part of mmap IPC sample provided under the MIT license
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was modified by Francis M. Columbus to operate a Seeed Studio RELAY SHIELD V3.0
 * https://www.seeedstudio.com/Relay-Shield-v3.0-p-2440.html
 *
 * Version 1.0 of the new modified code was built on November 19, 2016.
 *
 * The original version is found at:
 * https://software.intel.com/en-us/blogs/2014/09/22/efficient-communication-between-arduino-and-linux-native-processes
 */

/* mmap.cpp
 Linux native program communicating via memory mapped data with Arduino sketch.
 Compilation: g++  mmap.cpp   -lpthread -o mmap
 Run: ./mmap <RELAY1><RELAY2><RELAY3><RELAY4><LED13> (e.g. ./mmap 00001 -> RELAY 1-4 off, LED 13 on)
 For "random" blink you may run following commands in the command line:
 while [ 1 ]; do ./mmap $(($RANDOM % 2))$(($RANDOM % 2)); done
 */

#include "mmap.h"

void exitError(const char* errMsg) {
    perror(errMsg);
    exit(EXIT_FAILURE);
}


using namespace std;


/**
 * @brief: for this example uses a binary string "<relay1><RELAY2><RELAY3><RELAY4><led13>"; e.g. "11111": ALL on
 * if no arg equals "00000"
 * For "random" blink you may run following commands in the command line:
 * while [ 1 ]; do ./mmap $(($RANDOM % 2))$(($RANDOM % 2)); done
 */
int main(int argc, char** argv) {
    struct mmapData* p_mmapData; // here our mmapped data will be accessed
    int fd_mmapFile; // file descriptor for memory mapped file
    
    /* Create shared memory object and set its size */
    fd_mmapFile = open(mmapFilePath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd_mmapFile == -1) exitError("fd error; check errno for details");
    
    /* Map shared memory object read-writable */
    p_mmapData = static_cast<struct mmapData*>(mmap(NULL, sizeof(struct mmapData), PROT_READ | PROT_WRITE, MAP_SHARED, fd_mmapFile,
                                                    0));
    if (p_mmapData == MAP_FAILED) exitError("mmap error");
    /* the Arduino sketch might still be reading - by locking this program will be blocked until the mutex is unlocked from the
     reading sketch
     * in order to prevent race conditions */
    if (pthread_mutex_lock(&(p_mmapData->mutex)) != 0) exitError("pthread_mutex_lock");
    if (argc == 1) {
        cout << "7:0" << endl;
        cout << "6:0" << endl;
        cout << "5:0" << endl;
        cout << "4:0" << endl;
        cout << "13:0" << endl;
        p_mmapData->relay1_on = false;
        p_mmapData->relay2_on = false;
        p_mmapData->relay3_on = false;
        p_mmapData->relay4_on = false;
        p_mmapData->led13_on = false;
    }
    else if (argc > 1) {
        // assert(correct string given)
        int binNr = atol(argv[1]);
        if (binNr >= 10000) {
            cout << "7:1" << endl;
            p_mmapData->relay1_on = true;
        }
        else {
            cout << "7:0" << endl;
            p_mmapData->relay1_on = false;
        }
        binNr %= 10000;            // Keep only the lower 4 bits
        if (binNr >= 1000) {
            cout << "6:1" << endl;
            p_mmapData->relay2_on = true;
        }
        else {
            cout << "6:0" << endl;
            p_mmapData->relay2_on = false;
        }
        binNr %= 1000;            // Keep only the lower 3 bits
        if (binNr >= 100) {
            cout << "5:1" << endl;
            p_mmapData->relay3_on = true;
        }
        else {
            cout << "5:0" << endl;
            p_mmapData->relay3_on = false;
        }
        binNr %= 100;            // Keep only the lower 2 bits
        if (binNr >= 10) {
            cout << "4:1" << endl;
            p_mmapData->relay4_on = true;
        }
        else {
            cout << "4:0" << endl;
            p_mmapData->relay4_on = false;
        }
        binNr %= 10;            // Keep the lowest bit
        if (binNr == 1) {
            cout << "13:1" << endl;
            p_mmapData->led13_on = true;
        }
        else {
            cout << "13:0" << endl;
            p_mmapData->led13_on = false;
        }
    }
    // signal to waiting thread
    if (pthread_mutex_unlock(&(p_mmapData->mutex)) != 0) exitError("pthread_mutex_unlock");
    if (pthread_cond_signal(&(p_mmapData->cond)) != 0) exitError("pthread_cond_signal");
}

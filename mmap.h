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

#ifndef MMAP_HPP
#define MMAP_HPP

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <cstdio>
#include <pthread.h>

/* assert(/tmp mounted to tmpfs, i.e. resides in RAM) */
/* just use any file in /tmp */
static const char* mmapFilePath = "/tmp/arduino";


struct mmapData {
    bool relay1_on;   // relay1 on IO7
    bool relay2_on;   // relay2 on IO6
    bool relay3_on;   // relay3 on IO5
    bool relay4_on;   // relay4 on IO4
    bool led13_on;  // built-in led
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

#endif

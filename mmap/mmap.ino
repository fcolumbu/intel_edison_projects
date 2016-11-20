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

#include "mmap.h"

using namespace std;

/* assume /tmp mounted on /tmpfs -> all operation in memory */
/* we can use just any file in tmpfs. assert(file size not modified && file permissions left readable) */
struct mmapData* p_mmapData; // here our mmapped data will be accessed

int relay1 = 7;
int relay2 = 6;
int relay3 = 5;
int relay4 = 4;
int led13 = 13;


void exitError(const char* errMsg) {
  /* print to the serial Arduino is attached to, i.e. /dev/ttyGS0 */
  string s_cmd("echo 'error: ");
  s_cmd = s_cmd + errMsg + " - exiting' > /dev/ttyGS0";
  system(s_cmd.c_str()); 
  exit(EXIT_FAILURE);
}  

void setup() {
  int fd_mmapFile; // file descriptor for memory mapped file
  /* open file and mmap mmapData*/
  fd_mmapFile = open(mmapFilePath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd_mmapFile == -1) exitError("couldn't open mmap file"); 
  /* make the file the right size - exit if this fails*/
  if (ftruncate(fd_mmapFile, sizeof(struct mmapData)) == -1) exitError("couldn' modify mmap file");
  /* memory map the file to the data */
  /* assert(filesize not modified during execution) */
  p_mmapData = static_cast<struct mmapData*>(mmap(NULL, sizeof(struct mmapData), PROT_READ | PROT_WRITE, MAP_SHARED, fd_mmapFile, 0));  
  if (p_mmapData == MAP_FAILED) exitError("couldn't mmap"); 
  /* initialize mutex */
  pthread_mutexattr_t mutexattr; 
  if (pthread_mutexattr_init(&mutexattr) == -1) exitError("pthread_mutexattr_init");
  if (pthread_mutexattr_setrobust(&mutexattr, PTHREAD_MUTEX_ROBUST) == -1) exitError("pthread_mutexattr_setrobust");
  if (pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED) == -1) exitError("pthread_mutexattr_setpshared");
  if (pthread_mutex_init(&(p_mmapData->mutex), &mutexattr) == -1) exitError("pthread_mutex_init");

  /* initialize condition variable */
  pthread_condattr_t condattr;
  if (pthread_condattr_init(&condattr) == -1) exitError("pthread_condattr_init");
  if (pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED) == -1) exitError("pthread_condattr_setpshared");
  if (pthread_cond_init(&(p_mmapData->cond), &condattr) == -1) exitError("pthread_mutex_init");

  /* for this test we use 5 outputs */
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(led13, OUTPUT);
}

void loop() {
  /* block until we are signalled from native code */
  if (pthread_mutex_lock(&(p_mmapData->mutex)) != 0) exitError("pthread_mutex_lock");
  if (pthread_cond_wait(&(p_mmapData->cond), &(p_mmapData->mutex)) != 0) exitError("pthread_cond_wait");

  if (p_mmapData->relay1_on) {
    system("echo 7:1 > /dev/ttyGS0");
    digitalWrite(relay1, HIGH);
  }
  else {
    system("echo 7:0 > /dev/ttyGS0");
    digitalWrite(relay1, LOW);
  }  
    if (p_mmapData->relay2_on) {
    system("echo 6:1 > /dev/ttyGS0");
    digitalWrite(relay2, HIGH);
  }
  else {
    system("echo 6:0 > /dev/ttyGS0");
    digitalWrite(relay2, LOW);
  } 
    if (p_mmapData->relay3_on) {
    system("echo 5:1 > /dev/ttyGS0");
    digitalWrite(relay3, HIGH);
  }
  else {
    system("echo 5:0 > /dev/ttyGS0");
    digitalWrite(relay3, LOW);
  } 
    if (p_mmapData->relay4_on) {
    system("echo 4:1 > /dev/ttyGS0");
    digitalWrite(relay4, HIGH);
  }
  else {
    system("echo 4:0 > /dev/ttyGS0");
    digitalWrite(relay4, LOW);
  } 
  if (p_mmapData->led13_on) {
    system("echo 13:1 > /dev/ttyGS0");
    digitalWrite(led13, HIGH);
  }
  else {
    system("echo 13:0 > /dev/ttyGS0");
    digitalWrite(led13, LOW);
  }  
  if (pthread_mutex_unlock(&(p_mmapData->mutex)) != 0) exitError("pthread_mutex_unlock");
}


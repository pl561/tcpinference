//
// Created by Pascal Lefevre, march 6th 2022
//

/*
debug tips
- https://stackoverflow.com/questions/4552855/tcp-fragmentation


*/


#ifndef RAWBYTE_H
#define RAWBYTE_H

#include <vector>
#include <iostream>
#include <chrono>
#include <math.h>
#include <iomanip>
#include <fstream>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>


#define SIZE_LONG 8
#define SIZE_FLOAT 4
#define BUFFER_SIZE 4096

#define DEBUG0
#define READ_INFO
#define READ_INFO_LOOP
#define WRITE_INFO
#define WRITE_INFO_LOOP

#define READ_INFO_FLOAT
#define WRITE_INFO_FLOAT

#define READ_INFO_LONG
#define WRITE_INFO_LONG

#define READ_INFO_STRING
#define WRITE_INFO_STRING


// use C union and typedef
// use binary file to compare outputs in the tests


void error(const char *msg);

// template<typename T> void print_vector(std::vector<T>& v);
// void print_vector(std::vector<float> v, int num_floats);

// low-level read/write, these functions finish when 
// all the bytes have been sent/received
int write_byte_array(int sockfd, char *byte_array, ssize_t num_bytes);
int read_byte_array(int sockfd, char *byte_array, ssize_t num_bytes);



#endif
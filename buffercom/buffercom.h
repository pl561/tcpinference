//
// Created by Pascal Lefevre, march 6th 2022
//

#ifndef BUFFERCOM_H
#define BUFFERCOM_H

#include <vector>
#include <iostream>
#include <chrono>
#include <math.h>
#include <iomanip>
#include <fstream>
#include <algorithm>

#include "opencv2/core/mat.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>

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

// #define DEBUG0 1

// wraps a float vector with its shape
class Tensor {
    public:
    std::vector<long> shape;
    std::vector<float> tensor;

    Tensor();
    Tensor(std::vector<float> vec, std::vector<long> shape);
    Tensor(const Tensor& ts);
    ~Tensor();
    void setImageAsTensor(cv::Mat image);
    cv::Mat getImageFromTensor();
};

void error(const char *msg);
template<typename T> void print_vector(std::vector<T>& v);
void print_float_vector(std::vector<float> v, int num_floats);

int send_string(int sockfd, std::string str);
std::string receive_string(int sockfd);

int send_tensor_shape(int sockfd, std::vector<long> shape);
std::vector<long> receive_tensor_shape(int sockfd);

int write_float_array(int sockfd, float *array, long num_floats);
int read_float_array(int sockfd, float *array, long num_floats);

int write_long_array(int sockfd, long *array, long num_longs);
int read_long_array(int sockfd, long *array, long num_longs);

void send_float_vector(int sockfd, std::vector<float> tensor, std::vector<long> shape);
std::vector<float> receive_float_vector(int sockfd, std::vector<long> shape);

void send_tensor(int sockfd, Tensor ts);
Tensor receive_tensor(int sockfd);

// void send_float_tensor(int sockfd, std::vector<float> tensor, std::vector<long> shape);
// std::vector<float> receive_float_tensor(int sockfd);

// int send_float_raw_tensor(int sockfd, std::vector<float> tensor);
// std::vector<float> receive_float_raw_tensor(int sockfd, std::vector<long> shape);

std::vector<float> image_to_float_tensor(cv::Mat image);
cv::Mat float_tensor_to_image(std::vector<float> tensor, std::vector<long> shape);


class BufferComClient
{
    public:
    int sockfd;
    int portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    BufferComClient(char *addr, char *portno);
    ~BufferComClient();
    void connectClient();
    void closeClient();

    int sendString(std::string str);
    void sendFloatTensor(Tensor ts);
    
    std::string receiveString();
    Tensor receiveFloatTensor();
};

class BufferComServer
{
    public:
    int serversock, clientsock;
    int opt = 1;
    int portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    BufferComServer(char *addr, char *portno);
    ~BufferComServer();
    void bindListen();
    void acceptClient();
    void closeClient();
    void closeServer();

    int sendString(std::string str);
    void sendFloatTensor(Tensor ts);

    std::string receiveString();
    Tensor receiveFloatTensor();
};




#endif //BUFFERCOM_H

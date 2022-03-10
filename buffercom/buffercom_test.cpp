#include <algorithm>
#include <fstream>
#include <iomanip>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <sys/stat.h>
#include "gflags/gflags.h"
#include "buffercom.h"
#include "buffercom_test.h"
#include "rawbyte.h"


// test if the image conversion is correct using Tensor wrapper class
int test_tensor(int argc, char **argv) {
    cv::Mat image;
    image = cv::imread(argv[3], CV_LOAD_IMAGE_COLOR);
    if(!image.data)                              // Check for invalid input
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    std::vector<long> shape = {512, 512, 3};
    std::vector<float> tensor = image_to_float_tensor(image);
    cv::Mat image2 = float_tensor_to_image(tensor, shape);

    cv::imwrite("test_conversion.jpg", image2);

    Tensor t233(tensor, shape);
    cv::Mat rimg233 = t233.getImageFromTensor();
    cv::imwrite("tensorcheckconv233.jpg", rimg233);
    return 0;
}

///////// TO REFACTOR, obsolete code
// int test_client_server_v0(int argc, char **argv) {
//     // test v0:
//     // simple send receive of string, shape and vector
//     if (argc != 5) {
//        fprintf(stderr, "usage %s hostname port mode[0:client;1:server]\n", argv[0]);
//        exit(0);
//     }
//     int mode = atoi(argv[4]);
//     std::string str = "hello666.jpg";
//     cv::Mat image = cv::imread(argv[3], CV_LOAD_IMAGE_COLOR);
//     if(!image.data)                              // Check for invalid input
//     {
//         std::cout <<  "Could not open or find the image" << std::endl ;
//         return -1;
//     }
//     int h = 512;
//     int w = 512;
//     int c = 3;
//     std::vector<long> shape = {512, 512, 3};
//     int num_bytes = h * w * c * SIZE_FLOAT;
//     int num_floats = h * w * c;
//     std::vector<float> tensor = image_to_float_tensor(image);
//     // float *float_array = new float[h * w * c];
//     // for (int i = 0; i < h * w * c; i++) {
//     //     float_array[i] = tensor[i];
//     // }
//     if (mode == 0) {  // client mode
//         std::cout << "=== Client mode ===" << std::endl;
//         BufferComClient client = BufferComClient(argv[1], argv[2]);
//         client.connectClient();
//         client.sendString(str);
//         std::cout << "sending float tensor" << std::endl;
//         client.sendFloatTensor(tensor, shape);
//         std::cout << "sending float tensor: OK" << std::endl;
//         client.closeClient();    
//     }
//     else {  // server mode
//         std::cout << "=== Server mode ===" << std::endl;
//         BufferComServer server = BufferComServer(argv[1], argv[2]);
//         server.bindListen();
//         std::cout << "Listening. Waiting to accept a client..." << std::endl;
//         server.acceptClient();
//         std::string received_str = server.receiveString();
//         std::cout << "Received string: " << received_str << std::endl;
//         std::vector<float> rtensor = receive_float_vector(server.clientsock, shape);
//         for (int i = 0; i < 12; i++) {
//             // std::cout << ">> float:" << rtensor[i] << ":" << float_array2[i] << std::endl;
//             std::cout << ">> float:" << rtensor[i] << std::endl;
//         }
//         Tensor t(rtensor, shape);
//         cv::Mat rimg = t.getImageFromTensor();
//         if (rimg.empty()) {
//             std::cout << "empty image" << std::endl;
//         }
//         std::cout << rimg.rows << std::endl;
//         std::cout << rimg.cols << std::endl;
//         std::cout << rimg.channels() << std::endl;
//         cv::imwrite("server_received_image.jpg", rimg);
//         std::cout << "Close client" << std::endl;
//         server.closeClient();
//         std::cout << "Close server" << std::endl;
//         server.closeServer();
//     }
//     return 0;
// }

// test with send recv vector+shape
int test_client_server_v1(int argc, char **argv) {
    // gflags::SetUsageMessage(argv[0]);
    // gflags::ParseCommandLineFlags(&argc, &argv, true);
    // if(!path_check()){
    //     return -1;
    // }

    if (argc != 5) {
       fprintf(stderr, "usage %s hostname port mode[0:client;1:server]\n", argv[0]);
       exit(0);
    }
    int mode = atoi(argv[4]);

    std::string str = "hello666.jpg";
    cv::Mat image = cv::imread(argv[3], CV_LOAD_IMAGE_COLOR);
    if(!image.data)                              // Check for invalid input
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    // int h = 512;
    // int w = 512;
    // int c = 3;
    // std::vector<long> shape = {h, w, c};
    // int num_bytes = h * w * c * SIZE_FLOAT;
    // int num_floats = h * w * c;
    // std::vector<float> tensor = image_to_float_tensor(image);

    int h = 31;
    int w = 7;
    int c = 11;
    std::vector<long> shape = {h, w, c};
    long shapelen = shape.size();
    int num_floats = h * w * c;
    int num_bytes = num_floats * SIZE_FLOAT;
    std::vector<float> tensor(num_floats);
    for (int i = 0; i < 100; i++) {
        tensor[i] = i;
    }
    Tensor ts(tensor, shape);




    if (mode == 0) {  // client mode
        std::cout << "=== Client mode ===" << std::endl;
        BufferComClient client = BufferComClient(argv[1], argv[2]);
        client.connectClient();
        // client.sendString(str);

        // Test if the socket is in non-blocking mode:
        if(fcntl(client.sockfd, F_GETFL) & O_NONBLOCK) {
            std::cout << "socket is non blocking" << std::endl;
        }
        else {
            std::cout << "socket is blocking" << std::endl;

        }
        std::cout << "sending float tensor loop" << std::endl;

        for (int i = 0; i < 1; i++) {
            std::cout << "==== " << i << " ====" << std::endl;
            int sockfd = client.sockfd;

            // send
            std::cout << "==== " << "client send" << " ====" << std::endl;



            for (int j = 0; j < 100; j++) {
                printf("\nLOOP COUNT %i\n", j);
                // send_tensor_shape(sockfd, shape);
                // write_float_array(sockfd, tensor.data(), num_floats);

                // send_tensor(sockfd, ts);
                send_float_vector(sockfd, tensor, shape);

                std::vector<long> rshape = receive_tensor_shape(sockfd);
                int rnum_floats = 1; for (long l : rshape) rnum_floats *= l;
                float *rarray = new float[rnum_floats];
                read_float_array(sockfd, rarray, rnum_floats);
                std::vector<float> rtensor(rarray, rarray + rnum_floats);
                print_float_vector(rtensor, 12);
                printf("\nLOOP COUNT %i\n", j);
            }

            std::cout << std::endl;

        }
        
        std::cout << "sending float tensor loop: OK" << std::endl;

        client.closeClient();    
    }
    else {  // server mode
        std::cout << "=== Server mode ===" << std::endl;
        BufferComServer server = BufferComServer(argv[1], argv[2]);
        server.bindListen();
        std::cout << "Listening. Waiting to accept a client..." << std::endl;
        server.acceptClient();
        int sockfd = server.clientsock;



        for (int i = 0; i < 1; i++) {
            std::cout << "==== " << i << " ====" << std::endl;

            // receive
            std::cout << "==== " << "server receive" << " ====" << std::endl;



            float *rarray = new float[num_floats];
            for (int j = 0; j < 100; j++) {
                printf("\nLOOP COUNT %i\n", j);

                std::vector<long> rshape = receive_tensor_shape(sockfd);
                int rnum_floats = 1; for (long l : rshape) rnum_floats *= l;
                std::vector<float> rtensor = receive_float_vector(sockfd, shape);

                // read_float_array(sockfd, rarray, rnum_floats);
                // std::vector<float> rtensor(rarray, rarray + rnum_floats);

                print_float_vector(rtensor, 12);
                std::cout << std::endl;

                for (int k = 0; k < 100; k++) {
                    rarray[k] = k + 100 + 1;
                    if (k < 12) std::cout << rarray[k] << ", ";
                }
                std::vector<float> rtensor2(rarray, rarray + rnum_floats);
                std::cout << std::endl;
                send_float_vector(sockfd, rtensor2, rshape);
                // send_tensor_shape(sockfd, rshape);
                // write_float_array(sockfd, rarray, rnum_floats);
                printf("\nLOOP COUNT %i\n", j);
            }


        }


        std::cout << "Close client" << std::endl;
        server.closeClient();
        std::cout << "Close server" << std::endl;
        server.closeServer();
    }

    return 0;
}

int main(int argc, char **argv) {
    // gflags::SetUsageMessage(argv[0]);
    // gflags::ParseCommandLineFlags(&argc, &argv, true);
    // if(!path_check()){
    //     return -1;
    // }

    if (argc != 5) {
       fprintf(stderr, "usage %s hostname port mode[0:client;1:server]\n", argv[0]);
       exit(0);
    }
    int mode = atoi(argv[4]);

    std::string str = "hello666.jpg";
    cv::Mat image = cv::imread(argv[3], CV_LOAD_IMAGE_COLOR);
    if(!image.data)                              // Check for invalid input
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    int h = 31;
    int w = 7;
    int c = 11;
    std::vector<long> shape = {h, w, c};
    long shapelen = shape.size();
    int num_floats = h * w * c;
    int num_bytes = num_floats * SIZE_FLOAT;
    std::vector<float> tensor(num_floats);
    for (int i = 0; i < 100; i++) {
        tensor[i] = i;
    }
    Tensor ts(tensor, shape);


    if (mode == 0) {  // client mode
        std::cout << "=== Client mode ===" << std::endl;
        BufferComClient client = BufferComClient(argv[1], argv[2]);
        client.connectClient();
        // client.sendString(str);

        // Test if the socket is in non-blocking mode:
        if(fcntl(client.sockfd, F_GETFL) & O_NONBLOCK) {
            std::cout << "socket is non blocking" << std::endl;
        }
        else {
            std::cout << "socket is blocking" << std::endl;

        }
        std::cout << "sending float tensor loop" << std::endl;

        Tensor rts(ts);
        for (int i = 0; i < 1; i++) {
            int sockfd = client.sockfd;

            // send
            std::cout << "==== " << "client send" << " ====" << std::endl;

            for (int j = 0; j < 40; j++) {
                printf("\nLOOP COUNT %i\n", j);
                std::cout << "==== " << j << " ====" << std::endl;
                client.sendFloatTensor(rts);

                rts = client.receiveFloatTensor();
                print_float_vector(rts.tensor, 12);
                printf("\nLOOP COUNT %i\n", j);
            }

            std::cout << std::endl;

        }
        
        std::cout << "sending float tensor loop: OK" << std::endl;

        client.closeClient();    
    }
    else {  // server mode
        std::cout << "=== Server mode ===" << std::endl;
        BufferComServer server = BufferComServer(argv[1], argv[2]);
        server.bindListen();
        std::cout << "Listening. Waiting to accept a client..." << std::endl;
        server.acceptClient();
        int sockfd = server.clientsock;



        for (int i = 0; i < 1; i++) {
            std::cout << "==== " << i << " ====" << std::endl;

            // receive
            std::cout << "==== " << "server receive" << " ====" << std::endl;


            Tensor rts(ts);
            float *rarray = new float[num_floats];
            for (int j = 0; j < 40; j++) {
                printf("\nLOOP COUNT %i\n", j);
                std::cout << "==== " << j << " ====" << std::endl;
                rts = server.receiveFloatTensor();

                print_float_vector(rts.tensor, 12);
                std::cout << std::endl;
                std::vector<float> ts2(rts.tensor);
                for (int k = 0; k < 100; k++) {
                    ts2[k] = k * 2;
                    if (k < 12) std::cout << ts2[k] << ", ";
                }

                Tensor rts2(ts2, rts.shape);
                server.sendFloatTensor(rts2);
                printf("\nLOOP COUNT %i\n", j);
            }
        }

        std::cout << "Close client" << std::endl;
        server.closeClient();
        std::cout << "Close server" << std::endl;
        server.closeServer();
    }

    return 0;
}

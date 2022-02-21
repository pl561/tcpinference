#include "buffercom.h"


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

template<typename T>
void print_vector(std::vector<T>& v) {
    std::cout << "[";
    for (int i = 0; i < v.size(); i++) {
        std::cout << v[i] << ", ";
    }
    std::cout << "]";
}

void print_float_vector(std::vector<float> v, int num_floats) {
    std::cout << "[";
    for (int i = 0; i < v.size(); i++) {
        std::cout << v[i];
        if (i < v.size() - 1) std::cout << ", ";
        if (i >= num_floats) break;
    }
    std::cout << "]";
}

Tensor::Tensor(std::vector<float> vec, std::vector<long> sh) {
    tensor = vec;
    shape = sh;
}

Tensor::Tensor(const Tensor& ts) {
    tensor = ts.tensor;
    shape = ts.shape;
}

Tensor::~Tensor() {}

void Tensor::setImageAsTensor(cv::Mat image) {
    shape = {image.rows, image.cols, image.channels()};
    tensor = image_to_float_tensor(image);
}

cv::Mat Tensor::getImageFromTensor() {
    cv::Mat image = float_tensor_to_image(tensor, shape);
    return image;
}



int send_string(int sockfd, std::string str) {
    int status;
    size_t nbytes = str.length();
    long nbytesl = (long) nbytes;
    // send the string length as a long (8 bytes)
    status = write(sockfd, &nbytesl, SIZE_LONG);
    if (status < 0)
        error("ERROR send string length");

    // send the string byte by byte
    status = write(sockfd, str.c_str(), nbytes);
    if (status < 0)
        error("ERROR send string");
    return status;
}

std::string receive_string(int sockfd) {
    int status;
    long strlen;
    status = read(sockfd, &strlen, SIZE_LONG);
    std::cout << "status: " << status << std::endl;
    std::cout << "strlen: " << strlen << std::endl;
    if (status < 0)
        error("ERROR receive string length");

    // std::string received_str = "test not received any data from socket";
    // received_str.reserve(strlen);

    char *rstr = new char[strlen];

    status = read(sockfd, rstr, strlen);
    std::cout << "read string status: " << status << std::endl;
    for (int i = 0; i < strlen; i++) {
        std::cout << rstr[i];
    }
    std::cout << std::endl;
    if (status < 0)
        error("ERROR receive string");

    std::string received_str(rstr);
    return received_str;
}

int send_tensor_shape(int sockfd, std::vector<long> shape) {
    int status;
    long shapelen = shape.size();
    status = write_long_array(sockfd, &shapelen, 1);
    status = write_long_array(sockfd, shape.data(), shapelen);
    return status;
}

std::vector<long> receive_tensor_shape(int sockfd) {
    int status;
    // read the shape length
    long shapelen;
    status = read_long_array(sockfd, &shapelen, 1);
#ifdef DEBUG0
    std::cout << "shapelen: " << shapelen << "\n";
#endif
    // read each element of the shape
    long *long_array = new long[shapelen];
    status = read_long_array(sockfd, long_array, shapelen);
    if (status < 0)
        error("ERROR receive shape element");

    // store the shape elements inside a vector
    std::vector<long> shape(long_array, long_array + shapelen);
#ifdef DEBUG0
    for (auto elt : shape) std::cout << "receive_tensor_shape: " << elt << "\n";
#endif

    return shape;
}


// https://stackoverflow.com/questions/4552855/tcp-fragmentation
int read_float_array(int sockfd, float *array, long num_floats) {
    size_t num_bytes = num_floats * SIZE_FLOAT;
    char buffer[BUFFER_SIZE];
    char *byte_array = new char[num_bytes];
    ssize_t total = 0;

    // std::cout << "start recv" << std::endl;
    while (true) {
        int nbytes_left = num_bytes - total;
        ssize_t nb;
        if (nbytes_left < BUFFER_SIZE)
            nb = recv(sockfd, buffer, nbytes_left, 0);
        else
            nb = recv(sockfd, buffer, BUFFER_SIZE, 0);
        // copy byte by byte
        memcpy(byte_array + total, buffer, nb);

        total += nb;

#ifdef DEBUG0
            std::cout << "inloop float recv(total expected " << num_bytes << "): received: " << nb << " total: " << total << std::endl;
#endif
        if (nb == -1) {
            std::cout << "-1 float recv total: " << total << "// nbytes - total: " << num_bytes - total << std::endl;
            std::cout << "expected total num_bytes: " << num_bytes << std::endl;
            error( "recv float tensor failed" );
        }
        if (nb == 0) {
            std::cout << "break recv: " << total << std::endl;
            break;
        }
        if (total >= num_bytes) {
#ifdef DEBUG0
            std::cout << "break float recv total: " << total << std::endl;
#endif
            break;
        }
    }
    // copy float by float
    float *arr = (float *) byte_array;
    for (int i = 0; i < num_floats; i++)
        array[i] = arr[i];

    return total;
}

int write_float_array(int sockfd, float *array, long num_floats) {
    size_t num_bytes = num_floats * SIZE_FLOAT;
    ssize_t total = 0;
    // std::cout << "start send" << std::endl;
    char *byte_array = (char *) array;
    while (total < num_bytes) {
        int nbytes_left = num_bytes - total;
        ssize_t nb;
        if (nbytes_left < BUFFER_SIZE)
            nb = send(sockfd, byte_array + total, nbytes_left, 0);
        else
            nb = send(sockfd, byte_array + total, BUFFER_SIZE, 0);
        total += nb;
#ifdef DEBUG0
            std::cout << "inloop float write: " << nb << "// nbytes - total: " << num_bytes - total << std::endl;
#endif
        if ( nb == -1 ) error( "send float array failed" );
    }
    std::cout << "write_float_array: total byte sent:" << total << std::endl;
    return total;
}

int read_long_array(int sockfd, long *array, long num_longs) {
    size_t num_bytes = num_longs * SIZE_LONG;
    char buffer[BUFFER_SIZE];
    char *byte_array = new char[num_bytes];
    size_t total = 0;

    std::cout << "start recv long" << std::endl;
    while (true) {
        int nbytes_left = num_bytes - total;
        ssize_t nb;
        if (nbytes_left < BUFFER_SIZE)
            nb = recv(sockfd, buffer, nbytes_left, 0);
        else
            nb = recv(sockfd, buffer, BUFFER_SIZE, 0);

        memcpy(byte_array + total, buffer, nb);
        total += nb;
#ifdef DEBUG0
            std::cout << "inloop long recv(total expected " << num_bytes << "): received: " << nb << " total: " << total << std::endl;
#endif
        if (nb == -1) error("recv long tensor failed");
        if (nb == 0) {
            std::cout << "break recv 0: " << total << std::endl;
            break;
        }
        if (total == num_bytes) {
#ifdef DEBUG0
            std::cout << "break recv total: " << total << std::endl;
#endif
            break;
        }
    }
    // std::cout << ": bytes_read:" << total << std::endl;

    long *arr = (long *) byte_array;
    for (int i = 0; i < num_longs; i++)
        array[i] = arr[i];

    std::cout << "end recv long" << std::endl;
    return total;
}

int write_long_array(int sockfd, long *array, long num_longs) {
    size_t num_bytes = num_longs * SIZE_LONG;
    size_t total = 0;
    char *byte_array = (char *) array;
    while (total != num_bytes) {
        int nbytes_left = num_bytes - total;
        ssize_t nb;
        if (nbytes_left < BUFFER_SIZE)
            nb = send(sockfd, byte_array + total, nbytes_left, 0);
        else
            nb = send(sockfd, byte_array + total, BUFFER_SIZE, 0);
        total += nb;
#ifdef DEBUG0
            std::cout << "inloop long write: " << nb << "// nbytes - total: " << num_bytes - total << std::endl;
#endif
        if ( nb == -1 ) error( "send float array failed" );
    }
    std::cout << "write_long_array: total long send:" << total << std::endl;
    return total;
}


// send float tensor with shape
void send_float_vector(int sockfd, std::vector<float> tensor, std::vector<long> shape) {
#ifdef DEBUG0
    std::cout << "send tensor shape" << std::endl;
#endif
    send_tensor_shape(sockfd, shape);
    print_vector(shape);
    int num_floats = 1; 
    for (long l : shape) num_floats *= l;
    write_float_array(sockfd, tensor.data(), num_floats);
}

std::vector<float> receive_float_vector(int sockfd, std::vector<long> shape) {
    int num_floats = 1;
    for (long l : shape) num_floats *= l;
#ifdef DEBUG0
    std::cout << "num_floats: " << num_floats << std::endl;
#endif
    float *float_array = new float[num_floats];
    read_float_array(sockfd, float_array, num_floats);
    std::vector<float> tensor(float_array, float_array + num_floats);
    return tensor;
}

// send float tensor with shape
void send_tensor(int sockfd, Tensor ts) {
#ifdef DEBUG0
    std::cout << "send tensor shape" << std::endl;
#endif
    send_tensor_shape(sockfd, ts.shape);
    print_vector(ts.shape);
    int num_floats = 1; 
    for (long l : ts.shape) num_floats *= l;
    write_float_array(sockfd, ts.tensor.data(), num_floats);
}

Tensor receive_tensor(int sockfd) {
    std::vector<long> shape = receive_tensor_shape(sockfd);
    int num_floats = 1;
    for (long l : shape) num_floats *= l;
#ifdef DEBUG0
    std::cout << "num_floats: " << num_floats << std::endl;
#endif
    float *float_array = new float[num_floats];
    read_float_array(sockfd, float_array, num_floats);
    std::vector<float> tensor(float_array, float_array + num_floats);
    Tensor ts(tensor, shape);
    return ts;
}


// converts a uint8_t cv::Mat image
std::vector<float> image_to_float_tensor(cv::Mat image) {
    std::vector<float> tensor;

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            cv::Vec3b bgr = image.at<cv::Vec3b>(i, j);
            uint8_t b = bgr.val[0];
            uint8_t g = bgr.val[1];
            uint8_t r = bgr.val[2];

            float bf, rf, gf;
            bf = (float) b;
            gf = (float) g;
            rf = (float) r;

            tensor.push_back(bf);
            tensor.push_back(gf);
            tensor.push_back(rf);
        }
    }
    return tensor;
}

// shape format is HWC
cv::Mat float_tensor_to_image(std::vector<float> tensor, std::vector<long> shape) {
    cv::Size size(shape[1], shape[0]);
    cv::Mat image(size, CV_8UC3);
    int ofs = 0;
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            uint8_t b = (uint8_t) tensor[ofs];
            uint8_t g = (uint8_t) tensor[ofs + 1];
            uint8_t r = (uint8_t) tensor[ofs + 2];
            ofs += 3;
            image.at<cv::Vec3b>(i, j) = cv::Vec3b(b, g, r);
        }
    }
    return image;
}


// Client class wrapper implementation 
BufferComClient::BufferComClient(char *addr, char *port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(addr);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
        
    portno = atoi(port);
    serv_addr.sin_port = htons(portno);
}

BufferComClient::~BufferComClient() {
    closeClient();
}

void BufferComClient::connectClient() {
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    int flags = fcntl(sockfd, F_GETFL);
    flags &= ~O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags); 
}

void BufferComClient::closeClient() {
    close(sockfd);
}

int BufferComClient::sendString(std::string str) {
    return send_string(sockfd, str);
}

void BufferComClient::sendFloatTensor(Tensor ts) {
    send_tensor(sockfd, ts);
}

std::string BufferComClient::receiveString() {
    return receive_string(sockfd);
}

Tensor BufferComClient::receiveFloatTensor() {
    return receive_tensor(sockfd);
}

// Server class wrapper implementation 
BufferComServer::BufferComServer(char *addr, char *port) {
    serversock = socket(AF_INET, SOCK_STREAM, 0);
    if (serversock < 0) 
        error("ERROR opening socket");

    if (setsockopt(serversock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        error("ERROR set sockopt");
    }

    // https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
    // bind() of INADDR_ANY binds the socket to all available interfaces.
    bzero((char *) &serv_addr, sizeof(serv_addr));
    server = gethostbyname(addr);
    serv_addr.sin_family = AF_INET;
    // serv_addr.sin_addr.s_addr = inet_addr(addr);
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    portno = atoi(port);
    serv_addr.sin_port = htons(portno);
}

BufferComServer::~BufferComServer() {
    closeClient();
    closeServer();
}

void BufferComServer::closeClient() {
    close(serversock);
}

void BufferComServer::closeServer() {
    close(clientsock);
}

void BufferComServer::bindListen() {
    if (bind(serversock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("bind failed");
    }

    if (listen(serversock, 1) < 0)
    {
        error("listen failed");
    }
}

void BufferComServer::acceptClient() {
    int addrlen = sizeof(serv_addr);
    clientsock = accept(serversock, (struct sockaddr *)&serv_addr, (socklen_t*)&addrlen);
    if (clientsock < 0)
    {
        error("accept failed");
    }
}

int BufferComServer::sendString(std::string str) {
    return send_string(clientsock, str);
}

void BufferComServer::sendFloatTensor(Tensor ts) {
    send_tensor(clientsock, ts);
}

std::string BufferComServer::receiveString() {
    return receive_string(clientsock);
}

Tensor BufferComServer::receiveFloatTensor() {
    return receive_tensor(clientsock);
}

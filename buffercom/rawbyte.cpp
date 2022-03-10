#include "rawbyte.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// template<typename T>
// void print_vector(std::vector<T>& v) {
//     std::cout << "[";
//     for (int i = 0; i < v.size(); i++) {
//         std::cout << v[i] << ", ";
//     }
//     std::cout << "]";
// }

// void print_vector(std::vector<float> v, int num_el) {
//     std::cout << "[";
//     for (int i = 0; i < num_el; i++) {
//         std::cout << v[i];
//         if (i < v.size() - 1) std::cout << ", ";
//     }
//     std::cout << "]";
// }



/*
read byte array from sockfd
byte_array is a char point of an allocated
*/
int read_byte_array(int sockfd, char *byte_array, ssize_t num_bytes) {
    char buffer[BUFFER_SIZE];
    ssize_t total = 0;

#ifdef READ_INFO
            printf("[I] read_byte_array: start, total to receive %i bytes.\n", num_bytes);
#endif
    while (true) {
        int nbytes_left = num_bytes - total;
        ssize_t nb;
        if (nbytes_left < BUFFER_SIZE)
            nb = recv(sockfd, buffer, nbytes_left, 0);
        else
            nb = recv(sockfd, buffer, BUFFER_SIZE, 0);
        memcpy(byte_array + total, buffer, nb);

        total += nb;

#ifdef READ_INFO_LOOP
            printf("[I] read_byte_array: LOOP rec:%i/rem:%i/tot:%i bytes.\n", total, nbytes_left, num_bytes);
#endif
        if (nb == -1) {
            printf("[E] read_byte_array: break -1, expected total %i/%i bytes (remaining %i bytes).\n", total, num_bytes, num_bytes - total);
            error( "read_byte_array failed" );
        }
        if (nb == 0) {
            printf("[E] read_byte_array: break  0, received %i/%i bytes.\n", total, num_bytes);
            break;
        }
        if (total >= num_bytes) {
#ifdef READ_INFO
            printf("[I] read_byte_array: break, total received %i/%i bytes.\n", total, num_bytes);
#endif
            break;
        }
    }
    return total;
}

int write_byte_array(int sockfd, char *byte_array, ssize_t num_bytes) {
    ssize_t total = 0;
#ifdef READ_INFO
            printf("[I] write_byte_array: start, total to send %i bytes.\n", num_bytes);
#endif

    while (total < num_bytes) {
        int nbytes_left = num_bytes - total;
        ssize_t nb;
        if (nbytes_left < BUFFER_SIZE)
            nb = send(sockfd, byte_array + total, nbytes_left, 0);
        else
            nb = send(sockfd, byte_array + total, BUFFER_SIZE, 0);
        total += nb;
#ifdef WRITE_INFO_LOOP
            printf("[I] write_byte_array: LOOP rec:%i/rem:%i/tot:%i bytes.\n", total, nbytes_left, num_bytes);
#endif
        if (nb == -1) {
            printf("[E] write_byte_array: break -1, write %i/%i bytes (rem %i bytes).\n", total, num_bytes, num_bytes - total);
            error( "write_byte_array failed" );
        } 
    }

#ifdef WRITE_INFO
    printf("[I] write_byte_array: total sent %i/%i bytes.\n", total, num_bytes);
#endif
    return total;
}
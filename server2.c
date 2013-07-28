/*CO 318 Systems and Network Programming Laboratory and Project II
 *Project Title :..............Developing a reliable data transfer protocol for computer networks 
 *.............................and performance study
 * Registration Number :.......E/09/417
 *.............................Gunadasa M.M.G.S.M
 */

/*this server program connect with its client program using a UDP sockets
 * it will wait until the client program sends all the packets belongs to one 
 * window. after receiving all the packets belongs to the window server will send 
 * an acknowledgment to the client 
 * if all the packets received with out an error and with the correct order of the sequence numbers
 * the acknowledgment will be a positive acknowledgment. for a positive acknowledgment "ack_type" 
 * field in acknowledgment will be set to 1 
 * if there is any error negative acknowledgment will be send. the "ack_type" field 
 * will be set to 0. and the field "error_packet_no" will contain the sequence number of the 
 * firstly encountered error packet */

#include <math.h>
#include<time.h>
#include "headsock.h"
#include<sys/time.h>
void server(int, long);
void calculate_time(struct timeval*, struct timeval *);

int main(int argc, char **argv) {

    int socket_id, return_id;
    struct sockaddr_in my_address;
    long error_ratio;
    /*getting error ratio*/
    error_ratio = atof(argv[1]);
    
    /*creating socket */
    socket_id = socket(AF_INET, SOCK_DGRAM, 0);

    /*cheaking the socket*/
    if (socket_id < 0) {
        printf("error while creating the cocket\n");
        exit(1);

    }

    my_address.sin_family = AF_INET;
    my_address.sin_port = htons(MYUDP_PORT);
    my_address.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(my_address.sin_zero), 8);

    /*binding the socket */
    return_id = bind(socket_id, (struct sockaddr*) &my_address, sizeof (struct sockaddr));

    /*checking the bind */
    if (return_id < 0) {
        printf("error while binding\n");
        exit(1);
    }

    /*listening to the socket */
    printf("ready to listn\n");

    /*calling server function to handle the client*/
    server(socket_id, error_ratio);

    /*closing the socket and exiting */
    close(socket_id);
    exit(0);
}

/*this server function will handle the client.*/
void server(int socket_id, long error_r) {

    int receive_id = 0, lenght, send_id = 0, end = 1, received_data = 0, error_packet = 0, number_of_packets;
    int sq_root, random_number, is_error_detected = 0, number_of_error_packet = 0, error_packet_no = 0;
    struct ack_so ack;
    struct sockaddr_in addres;
    struct packet receving_packet;
    char buffer[BUFSIZE], result[BUFSIZE];
    long index = 0;
    float error_ratio = 0.0, total_time = 0.0;
    struct timeval start_t, end_t;
    lenght = sizeof (struct sockaddr_in);
    FILE *fp, *fp2;
    error_ratio = (float)error_r;
    receive_id = recvfrom(socket_id, &receving_packet, sizeof (struct packet), 0, (struct sockaddr *) &addres, &lenght);
    if (receive_id == -1) {
        printf("error while reciveing packets\n");
    }
    gettimeofday(&start_t, NULL);
    number_of_packets = receving_packet.no_of_packets;
    printf("number of packets about to received\t%d\n\n", number_of_packets);
    sq_root = sqrt(number_of_packets);

    /*waiting for the client to sent packets until it send the complete file */
    while (end) {
        /*receiving the packet*/
        receive_id = recvfrom(socket_id, &receving_packet, sizeof (struct packet), 0, (struct sockaddr *) &addres, &lenght);
        /*checking for the errors while sending*/
        if (receive_id == -1) {
            printf("error while receving packet\n");
        }

        /*if the client send everything exit from listing to the client*/
        if (receving_packet.seq_num == -1 && receving_packet.window_end == 1) {
            printf("end of the file\n");
            ack.ack_type = 1;
            /*acknowledging the client that every thing has received */
            send_id = sendto(socket_id, &ack, sizeof (struct ack_so), 0, (struct sockaddr*) &addres, lenght);
            if (send_id == -1) {
                printf("error while sendign ack\n");
            }
            end = 0;
            receive_id--;
            break;

        }
        /*generating the error packet no*/
        random_number = rand() % (int) sq_root;
        //  printf("error data\trand no %dsq root %d\n\n", random_number, sq_root);

        if ((random_number <= sq_root * error_ratio) && is_error_detected == 0) {
            printf("error packet is generated with seq no %d \n", receving_packet.seq_num);
            error_packet = receving_packet.seq_num;
            error_packet_no = receving_packet.packet_number;
            is_error_detected = 1;
            //printf("is error detected activated\n");
            number_of_error_packet++;
        }

        if (is_error_detected == 0) {

            /*determine the amount of data which should be read from the packet */
            received_data = strlen(receving_packet.data)-1;
            /*copping data to the buffer*/
            memmove((buffer + index), receving_packet.data, received_data);
            /*increasing the index of the buffer*/
            index += received_data;

            /*printing data about the receiving packet*/
            printf("received packet with seq no %d\t"
                    "and window end bit is %d\t"
                    "number of bit\t%d"
                    "data \t%d\n", receving_packet.seq_num, receving_packet.window_end, receive_id, strlen(receving_packet.data));

        }

        if (receving_packet.window_end == 1) {

            if (error_packet != 0) {
                /*error packet*/
                ack.ack_type = 0;
                ack.error_seq_no = error_packet;
                ack.error_packet = error_packet_no;
                printf("detected errors sending negative ack with error packet sequence number %d \n", ack.error_seq_no);
                send_id = sendto(socket_id, &ack, sizeof (struct ack_so), 0, (struct sockaddr*) &addres, lenght);
                if (send_id == -1) {
                    printf("ack sending fail\n");
                }
                error_packet = 0;
                is_error_detected = 0;
                // printf("is error detected disabled\n");
            } else {
                /*no errors*/
                printf("no errors sending positive ack\n");
                ack.ack_type = 1;
                send_id = sendto(socket_id, &ack, sizeof (struct ack_so), 0, (struct sockaddr*) &addres, lenght);
                if (send_id == -1) {
                    printf("ack sending fail\n");
                }

            }

        }

    }
    /*after receiving all the packets save it in a file*/
    fp = fopen("My_receve_file.txt", "wt");
    if (fp == NULL) {
        printf("file does not exsists\n");
        exit(0);
    }
    strcat(buffer, "\n");
    fwrite(buffer, 1, index+1, fp);
    fclose(fp);
     printf("file has completely saved\n");
    gettimeofday(&end_t, NULL);
    printf("start time %d end time %d\n", start_t.tv_sec, end_t.tv_sec);
    calculate_time(&start_t, &end_t);
    total_time += (end_t.tv_sec);
    
    
    
    
    printf("number of error packets %d\ntotal packets %d\n", number_of_error_packet, number_of_packets);
    printf("total time taken %.3f \n", total_time);

}

void calculate_time(struct timeval *start, struct timeval *end){
    
    if((start->tv_usec -= end->tv_usec)<0){
        --start->tv_sec;
        start->tv_usec += 1000000;
    }
    start->tv_sec -= end->tv_sec;
}



